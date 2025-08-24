// Fill out your copyright notice in the Description page of Project Settings.
#include "RTSPStreamManager.h"
#include "Engine/Texture2D.h"
#include "RenderUtils.h"
#include "Async/Async.h"
#include <thread>

extern "C" {
#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}




void URTSPStreamManager::StartStream(const FString& URL)
{
    bStreaming = true;
    DecodeThread = new std::thread([this, URL]() {
        InitFFmpeg(URL);
        });
}

void URTSPStreamManager::StopStream()
{
    bStreaming = false;

    if (DecodeThread)
    {
        if (DecodeThread->joinable())
        {
            DecodeThread->join();
        }

        delete DecodeThread;
        DecodeThread = nullptr;
    }

    // Clear Texture references
    AsyncTask(ENamedThreads::GameThread, [this]()
        {
            if (VideoUpdateRegion)
            {
                delete VideoUpdateRegion;
                VideoUpdateRegion = nullptr;
            }

            VideoTexture = nullptr;
        });
}

void URTSPStreamManager::BeginDestroy()
{
    bIsDestroyed = true;
    

    if (VideoUpdateRegion)
    {
        delete VideoUpdateRegion;
        VideoUpdateRegion = nullptr;
    }
    StopStream(); // Ensure safe shutdown
    Super::BeginDestroy();
}


void URTSPStreamManager::InitFFmpeg(const FString& URL)
{
    AVFormatContext* FormatCtx = nullptr;
    AVCodecContext* CodecCtx = nullptr;
    AVCodec* Codec = nullptr;
    AVFrame* Frame = nullptr;
    AVFrame* RGBFrame = nullptr;
    AVPacket* Packet = nullptr;
    SwsContext* SwsCtx = nullptr;

    // Open RTSP stream
    avformat_network_init();
    avformat_open_input(&FormatCtx, TCHAR_TO_ANSI(*URL), nullptr, nullptr);
    avformat_find_stream_info(FormatCtx, nullptr);
    int VideoStreamIndex = 0;
    for (unsigned i = 0; i < FormatCtx->nb_streams; ++i) {
        if (FormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            VideoStreamIndex = i;
            break;
        }
    }
    const AVCodec* CodecConst = avcodec_find_decoder(FormatCtx->streams[VideoStreamIndex]->codecpar->codec_id);
    Codec = const_cast<AVCodec*>(CodecConst);
    CodecCtx = avcodec_alloc_context3(Codec);
    avcodec_parameters_to_context(CodecCtx, FormatCtx->streams[VideoStreamIndex]->codecpar);
    avcodec_open2(CodecCtx, Codec, nullptr);

    Frame = av_frame_alloc();
    RGBFrame = av_frame_alloc();
    Packet = av_packet_alloc();

    int Width = CodecCtx->width;
    int Height = CodecCtx->height;

    // Create Unreal Texture
    AsyncTask(ENamedThreads::GameThread, [this, Width, Height]() {
        VideoTexture = UTexture2D::CreateTransient(Width, Height);
        VideoTexture->UpdateResource();
        VideoUpdateRegion = new FUpdateTextureRegion2D(0, 0, 0, 0, Width, Height);
        });

    // RGB buffer setup
    int NumBytes = av_image_get_buffer_size(AV_PIX_FMT_BGRA, Width, Height, 1);
    FrameBuffer.SetNumUninitialized(NumBytes);
    uint8_t* DestData[4] = { FrameBuffer.GetData(), nullptr, nullptr, nullptr };
    int DestLinesize[4] = { Width * 4, 0, 0, 0 };

    SwsCtx = sws_getContext(Width, Height, CodecCtx->pix_fmt,
        Width, Height, AV_PIX_FMT_BGRA,
        SWS_BILINEAR, nullptr, nullptr, nullptr);

    // Decode loop
    while (bStreaming && av_read_frame(FormatCtx, Packet) >= 0) {
        if (Packet->stream_index == VideoStreamIndex) {
            avcodec_send_packet(CodecCtx, Packet);
            while (avcodec_receive_frame(CodecCtx, Frame) == 0) {
                sws_scale(SwsCtx, Frame->data, Frame->linesize, 0, Height, DestData, DestLinesize);
                UpdateTexture(FrameBuffer.GetData(), Width, Height);
            }
        }
        av_packet_unref(Packet);
    }

    av_frame_free(&Frame);
    av_frame_free(&RGBFrame);
    av_packet_free(&Packet);
    avcodec_free_context(&CodecCtx);
    avformat_close_input(&FormatCtx);
}

void URTSPStreamManager::UpdateTexture(uint8* FrameData, int Width, int Height)
{
    if (!bStreaming || !VideoTexture.IsValid() || !VideoUpdateRegion)
        return;

    AsyncTask(ENamedThreads::GameThread, [this, FrameData, Width, Height]()
        {
            if (!bStreaming || !VideoTexture.IsValid() || !VideoUpdateRegion)
                return;

            VideoTexture->UpdateTextureRegions(0, 1, VideoUpdateRegion, Width * 4, 4, FrameData);
        });
}

