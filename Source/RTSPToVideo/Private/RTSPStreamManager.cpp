// RTSPStreamManager.cpp
//
// Crash fixes applied in this version:
//
//  [FIX-1] Sequential stop crash (4th stream after stop/start cycles):
//          Slot.MID was never nulled on StopStream in the widget, causing
//          a stale MID referencing a destroyed texture on restart.
//          → Widget's StopStream now fully resets MID + BoundTexture.
//          → UploadFrameToTexture guards against bStreaming=false entry.
//
//  [FIX-2] RHI dangling pointer (UpdateRegion freed while RHI still reading):
//          Original code passed Entry->UpdateRegion directly to
//          UpdateTextureRegions. RHI reads it asynchronously, so freeing
//          it in StopStream caused an assertion / crash.
//          → FRTSPUploadContext now owns a LOCAL COPY of Region + Buffer.
//          → RHI only ever touches FRTSPUploadContext — never FRTSPStreamEntry.
//          → StopStream can free Entry->UpdateRegion right after join() safely.
//
//  [FIX-3] FlushRenderingCommands() freezes simultaneous streams:
//          Calling FlushRenderingCommands() blocks the Game Thread and
//          stalls all other playing streams.
//          → Removed entirely. FRTSPUploadContext ref-counting replaces it.
//
//  [FIX-4] URL encoding (%40 etc.) causing immediate FFmpeg disconnect:
//          TCHAR_TO_ANSI drops non-ASCII and doesn't decode percent-encoding.
//          → URL is pre-decoded and passed as UTF-8 to FFmpeg.
//
//  [FIX-5] TickStreams processing stopped entries:
//          After StopStream removes entry from map, TickStreams could still
//          be mid-iteration on that entry (lock held).
//          → bStreaming guard added at top of TickStreams loop and inside
//          UploadFrameToTexture.

#include "RTSPStreamManager.h"
#include "RenderUtils.h"
#include "Async/Async.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}

// ─────────────────────────────────────────────────────────────
//  Helper: decode percent-encoded characters in RTSP URLs
//  e.g. Arth%40123 → Arth@123
// ─────────────────────────────────────────────────────────────
static FString DecodeURLEncoding(const FString& URL)
{
    FString Result = URL;
    Result = Result.Replace(TEXT("%40"), TEXT("@"));
    Result = Result.Replace(TEXT("%3A"), TEXT(":"));
    Result = Result.Replace(TEXT("%2F"), TEXT("/"));
    Result = Result.Replace(TEXT("%3F"), TEXT("?"));
    Result = Result.Replace(TEXT("%3D"), TEXT("="));
    Result = Result.Replace(TEXT("%26"), TEXT("&"));
    Result = Result.Replace(TEXT("%20"), TEXT(" "));
    return Result;
}

// ─────────────────────────────────────────────────────────────
//  StartStream
// ─────────────────────────────────────────────────────────────
int32 URTSPStreamManager::StartStream(const FString& URL)
{
    if (URL.IsEmpty())
    {
        UE_LOG(LogTemp, Error, TEXT("RTSP RTSPManager: Empty URL"));
        return -1;
    }

    TUniquePtr<FRTSPStreamEntry> Entry = MakeUnique<FRTSPStreamEntry>();
    Entry->URL = URL;
    Entry->bStreaming.store(true);
    Entry->bThreadFinished.store(false);

    // Capture raw ptr — safe because entry lives until StopStream joins the thread
    FRTSPStreamEntry* RawEntry = Entry.Get();
    RawEntry->DecodeThread = std::thread([this, RawEntry]()
    {
        DecodeLoop(RawEntry);
        RawEntry->bThreadFinished.store(true);
    });

    FScopeLock Lock(&StreamsLock);
    int32 ID = NextStreamID++;
    Streams.Add(ID, MoveTemp(Entry));

    UE_LOG(LogTemp, Log, TEXT("RTSP RTSPManager: Started stream %d → %s, Streams array Num: %d"), ID, *URL, Streams.Num());
    return ID;
}

// ─────────────────────────────────────────────────────────────
//  StopStream
//
//  Execution order is critical:
//    1. Set bStreaming=false  (decode loop exits on next iteration)
//    2. Remove from map       (TickStreams can never reach this entry again)
//    3. Join decode thread    (guarantees no more FrameBuffer writes)
//    4. Clear FrameBuffer     (release memory)
//    5. Free UpdateRegion     (safe — RHI holds FRTSPUploadContext, not this)
//    6. Null VideoTexture     (UObject ref released)
//    7. TUniquePtr destroyed  (all heap memory freed)
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::StopStream(int32 StreamID)
{
    TUniquePtr<FRTSPStreamEntry> OwnedEntry;

    {
        FScopeLock Lock(&StreamsLock);
        TUniquePtr<FRTSPStreamEntry>* Found = Streams.Find(StreamID);
        if (!Found)
        {
            UE_LOG(LogTemp, Warning,
                TEXT("RTSP RTSPManager: StopStream(%d) — ID not found"), StreamID);
            return;
        }

        // Step 1: signal decode loop to exit
        (*Found)->bStreaming.store(false);

        // Step 2: remove from map — TickStreams won't touch this entry again
        OwnedEntry = MoveTemp(*Found);
        Streams.Remove(StreamID);
    }
    // Lock released — TickStreams can run freely for remaining streams

    // Step 3: join — guarantees decode thread has fully exited
    // (no more sws_scale writes into FrameBuffer after this returns)
    if (OwnedEntry->DecodeThread.joinable())
    {
        OwnedEntry->DecodeThread.join();
    }

    // Step 4: clear frame buffer
    {
        FScopeLock Lock(&OwnedEntry->FrameBufferLock);
        OwnedEntry->bNewFrame   = false;
        OwnedEntry->FrameWidth  = 0;
        OwnedEntry->FrameHeight = 0;
        OwnedEntry->FrameBuffer.Empty();
    }

    // Step 5: free UpdateRegion
    // SAFE because:
    //   • Decode thread has joined → no more FrameBuffer writes
    //   • Entry removed from map → TickStreams won't call UploadFrameToTexture
    //   • RHI holds FRTSPUploadContext (its own Region copy), NOT this pointer
    //   • No FlushRenderingCommands() needed — other streams are unaffected
    if (OwnedEntry->UpdateRegion)
    {
        delete OwnedEntry->UpdateRegion;
        OwnedEntry->UpdateRegion = nullptr;
    }

    // Step 6: release UObject texture reference
    OwnedEntry->VideoTexture = nullptr;

    UE_LOG(LogTemp, Log, TEXT("RTSP RTSPManager: Stream %d stopped"), StreamID);

    // Step 7: OwnedEntry goes out of scope → TUniquePtr frees FRTSPStreamEntry
}

// ─────────────────────────────────────────────────────────────
//  StopAllStreams
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::StopAllStreams()
{
    TArray<int32> IDs;
    {
        FScopeLock Lock(&StreamsLock);
        Streams.GetKeys(IDs);
    }

    for (int32 ID : IDs)
    {
        StopStream(ID);
    }
}

// ─────────────────────────────────────────────────────────────
//  TickStreams  —  Game Thread only, called every frame
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::TickStreams()
{
    check(IsInGameThread());

    // Streams whose first frame arrived this tick — broadcast AFTER the lock
    // is released so handlers can safely call back into the manager.
    TArray<int32, TInlineAllocator<4>> FirstFrameStreamIDs;

    {
        FScopeLock Lock(&StreamsLock);

        for (auto& Pair : Streams)
        {
            FRTSPStreamEntry* Entry = Pair.Value.Get();
            if (!Entry) continue;

            // Skip entries that are in the process of stopping
            if (!Entry->bStreaming.load()) continue;

            // ── Create texture once video dimensions are known ────
            if (!Entry->VideoTexture && Entry->FrameWidth > 0 && Entry->FrameHeight > 0)
            {
                Entry->VideoTexture = UTexture2D::CreateTransient(
                    Entry->FrameWidth, Entry->FrameHeight, PF_B8G8R8A8);

                if (Entry->VideoTexture)
                {
                    Entry->VideoTexture->UpdateResource();

                    // UpdateRegion stores dimensions only.
                    // It is NEVER passed to the RHI directly.
                    Entry->UpdateRegion = new FUpdateTextureRegion2D(
                        0, 0, 0, 0, Entry->FrameWidth, Entry->FrameHeight);

                    UE_LOG(LogTemp, Log,
                        TEXT("RTSP RTSPManager: Texture created %dx%d for stream"),
                        Entry->FrameWidth, Entry->FrameHeight);
                }
            }

            // ── Upload new frame if texture is ready ──────────────
            if (Entry->VideoTexture && Entry->UpdateRegion && Entry->bStreaming.load())
            {
                UploadFrameToTexture(Entry);

                if (Entry->bFirstFrameUploaded && !Entry->bFirstFrameBroadcast)
                {
                    Entry->bFirstFrameBroadcast = true;
                    FirstFrameStreamIDs.Add(Pair.Key);
                }
            }
        }
    }

    for (int32 StreamID : FirstFrameStreamIDs)
    {
        UE_LOG(LogTemp, Log, TEXT("RTSP RTSPManager: First frame ready for stream %d"), StreamID);
        OnStreamFirstFrame.Broadcast(StreamID);
    }
}

// ─────────────────────────────────────────────────────────────
//  UploadFrameToTexture  —  Game Thread only
//
//  Key design: FRTSPUploadContext owns its own Region copy and
//  pixel buffer. The RHI thread only ever reads from Ctx — it
//  never touches FRTSPStreamEntry. This is what makes StopStream
//  safe without FlushRenderingCommands().
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::UploadFrameToTexture(FRTSPStreamEntry* Entry)
{
    check(IsInGameThread());

    // ── Guards ────────────────────────────────────────────────
    if (!Entry)                        return;
    if (!Entry->bStreaming.load())     return;
    if (!Entry->bNewFrame)             return;  // fast pre-check without lock
    if (!Entry->VideoTexture)          return;
    if (!Entry->UpdateRegion)          return;

    TArray<uint8> LocalBuffer;
    int32 W = 0, H = 0;

    {
        FScopeLock Lock(&Entry->FrameBufferLock);

        // Re-check inside lock — decode thread may have cleared it
        if (!Entry->bNewFrame)         return;
        if (!Entry->bStreaming.load()) return;

        LocalBuffer      = Entry->FrameBuffer;   // copy pixel data
        W                = Entry->FrameWidth;
        H                = Entry->FrameHeight;
        Entry->bNewFrame = false;                 // consumed
    }

    if (LocalBuffer.Num() < W * H * 4) return;
    if (!Entry)
    {
        UE_LOG(LogTemp, Error, TEXT("Entry is null"));
        return;
    }

    if (!IsValid(Entry->VideoTexture))
    {
        UE_LOG(LogTemp, Error, TEXT("VideoTexture is invalid"));
        return;
    }
    // ── Validate dimensions against actual texture size ───────
    int32 TexW = Entry->VideoTexture->GetSizeX();
    int32 TexH = Entry->VideoTexture->GetSizeY();

    if (W <= 0 || H <= 0 || TexW <= 0 || TexH <= 0) return;

    if (W != TexW || H != TexH)
    {
        // Frame dimensions changed mid-stream — recreate texture next tick
        UE_LOG(LogTemp, Warning,
            TEXT("RTSP RTSPManager: Dimension mismatch frame=(%dx%d) tex=(%dx%d) — recreating"),
            W, H, TexW, TexH);

        delete Entry->UpdateRegion;
        Entry->UpdateRegion = nullptr;
        Entry->VideoTexture = nullptr;  // triggers recreation in TickStreams next tick
        return;
    }

    // ── Build self-contained upload context ───────────────────
    //
    //  Ctx owns:
    //    • Ctx->Buffer  — pixel data (moved in, no copy)
    //    • Ctx->Region  — local FUpdateTextureRegion2D (NOT Entry->UpdateRegion)
    //
    //  The RHI thread reads Ctx->Buffer.GetData() and &Ctx->Region.
    //  When it finishes, the ActualRenderingThread task calls Ctx->Release().
    //  If RefCount hits 0, Ctx deletes itself.
    //
    //  Entry->UpdateRegion is never touched by the RHI → safe to delete
    //  in StopStream immediately after DecodeThread.join().
    //
    FRTSPUploadContext* Ctx = new FRTSPUploadContext();
    Ctx->Buffer         = MoveTemp(LocalBuffer);  // zero-copy move
    Ctx->Region.SrcX    = 0;
    Ctx->Region.SrcY    = 0;
    Ctx->Region.DestX   = 0;
    Ctx->Region.DestY   = 0;
    Ctx->Region.Width   = (uint32)TexW;
    Ctx->Region.Height  = (uint32)TexH;

    Entry->VideoTexture->UpdateTextureRegions(
        0,                          // mip index
        1,                          // region count
        &Ctx->Region,               // ← Ctx's own copy, NOT Entry->UpdateRegion
        (uint32)(TexW * 4),         // source pitch (bytes per row)
        4,                          // bytes per pixel
        Ctx->Buffer.GetData(),
        [](uint8*, const FUpdateTextureRegion2D*) {}  // no-op; we manage lifetime below
    );

    // Release Ctx on the rendering thread — after RHI has consumed the data
    AsyncTask(ENamedThreads::ActualRenderingThread, [Ctx]()
    {
        Ctx->Release();
    });

    // A real decoded frame has now been pushed to the texture —
    // TickStreams broadcasts OnStreamFirstFrame once per stream from this.
    Entry->bFirstFrameUploaded = true;
}

// ─────────────────────────────────────────────────────────────
//  AddReferencedObjects
//
//  The Streams map is a plain TMap of TUniquePtr — invisible to
//  reflection — so each entry's VideoTexture must be reported to
//  the GC manually or it gets collected while the stream is live.
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector)
{
    URTSPStreamManager* This = CastChecked<URTSPStreamManager>(InThis);

    {
        FScopeLock Lock(&This->StreamsLock);
        for (auto& Pair : This->Streams)
        {
            FRTSPStreamEntry* Entry = Pair.Value.Get();
            if (Entry && Entry->VideoTexture)
            {
                Collector.AddReferencedObject(Entry->VideoTexture);
            }
        }
    }

    Super::AddReferencedObjects(InThis, Collector);
}

// ─────────────────────────────────────────────────────────────
//  GetVideoTexture
// ─────────────────────────────────────────────────────────────
UTexture2D* URTSPStreamManager::GetVideoTexture(int32 StreamID) const
{
    FScopeLock Lock(&StreamsLock);
    const TUniquePtr<FRTSPStreamEntry>* Found = Streams.Find(StreamID);
    if (!Found) return nullptr;
    return (*Found)->VideoTexture;
}

// ─────────────────────────────────────────────────────────────
//  GetActiveStreamCount
// ─────────────────────────────────────────────────────────────
int32 URTSPStreamManager::GetActiveStreamCount() const
{
    FScopeLock Lock(&StreamsLock);
    return Streams.Num();
}

// ─────────────────────────────────────────────────────────────
//  BeginDestroy
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::BeginDestroy()
{
    StopAllStreams();  // joins every decode thread before any memory is freed
    Super::BeginDestroy();
}

// ─────────────────────────────────────────────────────────────
//  DecodeLoop  —  runs on a dedicated std::thread per stream
// ─────────────────────────────────────────────────────────────
void URTSPStreamManager::DecodeLoop(FRTSPStreamEntry* Entry)
{
    avformat_network_init();

    // ── Decode percent-encoded characters in URL ──────────────
    // e.g. rtsp://user:Pass%40word@ip/  →  rtsp://user:Pass@word@ip/
    FString DecodedURL = DecodeURLEncoding(Entry->URL);

    AVFormatContext* FormatCtx = nullptr;
    AVDictionary*    Opts      = nullptr;

    // Low-latency RTSP options
    av_dict_set(&Opts, "rtsp_transport", "tcp",      0);
    av_dict_set(&Opts, "stimeout",       "3000000",  0);  // 3 s connection timeout
    av_dict_set(&Opts, "max_delay",      "500000",   0);  // 0.5 s max queue delay

    // Use UTF-8 encoding — handles special characters that ANSI drops
    int OpenResult = avformat_open_input(
        &FormatCtx, TCHAR_TO_UTF8(*DecodedURL), nullptr, &Opts);
    av_dict_free(&Opts);

    if (OpenResult < 0)
    {
        char ErrBuf[256];
        av_strerror(OpenResult, ErrBuf, sizeof(ErrBuf));
        UE_LOG(LogTemp, Error,
            TEXT("RTSP RTSPManager: Cannot open %s — %s"),
            *Entry->URL, UTF8_TO_TCHAR(ErrBuf));
        Entry->bStreaming.store(false);
        return;
    }

    if (avformat_find_stream_info(FormatCtx, nullptr) < 0)
    {
        UE_LOG(LogTemp, Error,
            TEXT("RTSP RTSPManager: No stream info — %s"), *Entry->URL);
        avformat_close_input(&FormatCtx);
        Entry->bStreaming.store(false);
        return;
    }

    // ── Find video stream ─────────────────────────────────────
    int VideoStreamIndex = -1;
    for (unsigned i = 0; i < FormatCtx->nb_streams; ++i)
    {
        if (FormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO)
        {
            VideoStreamIndex = (int)i;
            break;
        }
    }
    if (VideoStreamIndex < 0)
    {
        UE_LOG(LogTemp, Error,
            TEXT("RTSP RTSPManager: No video stream — %s"), *Entry->URL);
        avformat_close_input(&FormatCtx);
        Entry->bStreaming.store(false);
        return;
    }

    // ── Open codec ────────────────────────────────────────────
    const AVCodec* Codec = avcodec_find_decoder(
        FormatCtx->streams[VideoStreamIndex]->codecpar->codec_id);
    if (!Codec)
    {
        UE_LOG(LogTemp, Error, TEXT("RTSP RTSPManager: Codec not found — %s"), *Entry->URL);
        avformat_close_input(&FormatCtx);
        Entry->bStreaming.store(false);
        return;
    }

    AVCodecContext* CodecCtx = avcodec_alloc_context3(Codec);
    avcodec_parameters_to_context(CodecCtx, FormatCtx->streams[VideoStreamIndex]->codecpar);

    // Multi-threaded decode — reduces per-stream CPU overhead for 20+ streams
    CodecCtx->thread_count = 2;
    CodecCtx->thread_type  = FF_THREAD_FRAME;

    if (avcodec_open2(CodecCtx, Codec, nullptr) < 0)
    {
        UE_LOG(LogTemp, Error, TEXT("RTSP RTSPManager: Cannot open codec — %s"), *Entry->URL);
        avcodec_free_context(&CodecCtx);
        avformat_close_input(&FormatCtx);
        Entry->bStreaming.store(false);
        return;
    }

    int32 Width  = CodecCtx->width;
    int32 Height = CodecCtx->height;

    // Publish dimensions → TickStreams will create the texture on the GT
    {
        FScopeLock Lock(&Entry->FrameBufferLock);
        Entry->FrameWidth  = Width;
        Entry->FrameHeight = Height;
        Entry->FrameBuffer.SetNumUninitialized(Width * Height * 4);
    }

    SwsContext* SwsCtx = sws_getContext(
        Width, Height, CodecCtx->pix_fmt,
        Width, Height, AV_PIX_FMT_BGRA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    AVFrame*  Frame  = av_frame_alloc();
    AVPacket* Packet = av_packet_alloc();

    uint8_t* DestData[4]     = { nullptr };
    int      DestLinesize[4] = { 0 };

    // ── Decode loop ───────────────────────────────────────────
    while (Entry->bStreaming.load())
    {
        int ReadResult = av_read_frame(FormatCtx, Packet);
        if (ReadResult < 0)
        {
            char ErrBuf[128];
            av_strerror(ReadResult, ErrBuf, sizeof(ErrBuf));
            UE_LOG(LogTemp, Warning,
                TEXT("RTSP RTSPManager: av_read_frame: %s (%s)"),
                UTF8_TO_TCHAR(ErrBuf), *Entry->URL);
            break;
        }

        if (Packet->stream_index == VideoStreamIndex)
        {
            if (avcodec_send_packet(CodecCtx, Packet) == 0)
            {
                while (avcodec_receive_frame(CodecCtx, Frame) == 0)
                {
                    // Write BGRA frame into shared buffer under lock
                    {
                        FScopeLock Lock(&Entry->FrameBufferLock);

                        // Abort write if StopStream was called mid-frame
                        if (!Entry->bStreaming.load()) break;

                        DestData[0]     = Entry->FrameBuffer.GetData();
                        DestLinesize[0] = Width * 4;

                        sws_scale(SwsCtx,
                            Frame->data, Frame->linesize,
                            0, Height,
                            DestData, DestLinesize);

                        Entry->bNewFrame = true;
                    }
                    av_frame_unref(Frame);
                }
            }
        }
        av_packet_unref(Packet);
    }

    // ── FFmpeg cleanup (decode thread owns all FFmpeg objects) ─
    sws_freeContext(SwsCtx);
    av_frame_free(&Frame);
    av_packet_free(&Packet);
    avcodec_free_context(&CodecCtx);
    avformat_close_input(&FormatCtx);

    UE_LOG(LogTemp, Log,
        TEXT("RTSP RTSPManager: Decode thread exited — %s"), *Entry->URL);
}
