#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "Engine/Texture2D.h"
#include <thread>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include "RTSPStreamManager.generated.h"

struct FUpdateTextureRegion2D;

/** Fired on the Game Thread the first time a decoded frame is uploaded
 *  to a stream's texture — i.e. the moment video actually becomes visible. */
DECLARE_MULTICAST_DELEGATE_OneParam(FOnRTSPStreamFirstFrame, int32 /*StreamID*/);

// ─────────────────────────────────────────────────────────────
//  FRTSPUploadContext
//
//  Owns EVERYTHING the RHI thread needs for one texture upload:
//    • a local copy of the pixel buffer
//    • a local copy of the update region
//
//  Because the RHI only ever touches this struct (never the
//  FRTSPStreamEntry), StopStream() can free the entry immediately
//  after joining the decode thread — no FlushRenderingCommands()
//  stall, no freeze of other playing streams.
//
//  Ref-counted: freed automatically when both the Game Thread
//  and the Rendering Thread have finished with it.
// ─────────────────────────────────────────────────────────────
struct FRTSPUploadContext
{
    TArray<uint8>           Buffer;  // pixel data (BGRA)
    FUpdateTextureRegion2D  Region;  // local copy — NOT a pointer into FRTSPStreamEntry

    FThreadSafeCounter      RefCount;

    FRTSPUploadContext()  { RefCount.Increment(); }

    void AddRef()  { RefCount.Increment(); }

    void Release()
    {
        if (RefCount.Decrement() == 0)
        {
            delete this;
        }
    }

private:
    FRTSPUploadContext(const FRTSPUploadContext&)            = delete;
    FRTSPUploadContext& operator=(const FRTSPUploadContext&) = delete;
};

// ─────────────────────────────────────────────────────────────
//  FRTSPStreamEntry  –  all state for one RTSP feed
// ─────────────────────────────────────────────────────────────
struct FRTSPStreamEntry
{
    FString          URL;

    // ── Texture (Game Thread only) ────────────────────────────
    // TObjectPtr (not raw) so AddReferencedObjects can report it to the GC
    // safely under incremental garbage collection.
    TObjectPtr<UTexture2D> VideoTexture = nullptr;

    // UpdateRegion is used only to store the texture dimensions.
    // It is NEVER passed directly to UpdateTextureRegions —
    // each upload call makes its own copy inside FRTSPUploadContext.
    FUpdateTextureRegion2D* UpdateRegion = nullptr;

    // ── Frame data (shared, guarded by FrameBufferLock) ───────
    TArray<uint8>    FrameBuffer;
    FCriticalSection FrameBufferLock;  // guards FrameBuffer, bNewFrame
    bool             bNewFrame   = false;
    int32            FrameWidth  = 0;
    int32            FrameHeight = 0;

    // ── First-frame tracking (Game Thread only) ───────────────
    bool             bFirstFrameUploaded  = false;  // set by UploadFrameToTexture
    bool             bFirstFrameBroadcast = false;  // set by TickStreams after broadcast

    // ── Life-cycle (atomic — readable from any thread) ────────
    std::atomic<bool> bStreaming      {false};
    std::atomic<bool> bThreadFinished {false};

    // ── Decode thread ─────────────────────────────────────────
    std::thread      DecodeThread;

    // Non-copyable
    FRTSPStreamEntry() = default;
    FRTSPStreamEntry(const FRTSPStreamEntry&)            = delete;
    FRTSPStreamEntry& operator=(const FRTSPStreamEntry&) = delete;
};

// ─────────────────────────────────────────────────────────────
//  URTSPStreamManager
// ─────────────────────────────────────────────────────────────
UCLASS()
class RTSPTOVIDEO_API URTSPStreamManager : public UObject
{
    GENERATED_BODY()

public:
    /**
     * Start streaming from URL.
     * Returns a StreamID used for all subsequent calls.
     * Returns -1 on failure (e.g. empty URL).
     */
    int32 StartStream(const FString& URL);

    /**
     * Stop a single stream by ID. Call from the Game Thread.
     *
     * Crash-safe for BOTH use cases:
     *   • Sequential  – start/stop one stream at a time
     *   • Simultaneous – stop one while others keep playing
     *
     * How it achieves this without FlushRenderingCommands():
     *   1. Removes entry from the map → TickStreams can't reach it.
     *   2. Sets bStreaming=false     → decode loop exits.
     *   3. Joins decode thread       → no more frame writes.
     *   4. Frees UpdateRegion/Texture immediately after join.
     *      Safe because the RHI only ever holds FRTSPUploadContext
     *      pointers — it never holds a pointer into the entry.
     */
    void StopStream(int32 StreamID);

    /** Stop all active streams. */
    void StopAllStreams();

    /**
     * Call every frame from the Game Thread (e.g. NativeTick).
     * Creates textures once video dimensions are known and
     * uploads newly decoded frames.
     */
    void TickStreams();

    /** Returns nullptr if StreamID is invalid or texture not ready yet. */
    UTexture2D* GetVideoTexture(int32 StreamID) const;

    /** Broadcast once per stream when its first decoded frame reaches the texture.
     *  Fired on the Game Thread from TickStreams (outside the streams lock). */
    FOnRTSPStreamFirstFrame OnStreamFirstFrame;

    /** Number of currently active (non-stopped) streams. */
    int32 GetActiveStreamCount() const;

    virtual void BeginDestroy() override;

    /** GC hook — VideoTexture pointers live in a non-reflected TMap, so without
     *  this the transient textures have zero GC-visible references and get
     *  collected mid-stream (crash in UploadFrameToTexture). */
    static void AddReferencedObjects(UObject* InThis, FReferenceCollector& Collector);

private:
    void DecodeLoop(FRTSPStreamEntry* Entry);
    void UploadFrameToTexture(FRTSPStreamEntry* Entry);

    mutable FCriticalSection StreamsLock;
    TMap<int32, TUniquePtr<FRTSPStreamEntry>> Streams;
    int32 NextStreamID = 0;
};
