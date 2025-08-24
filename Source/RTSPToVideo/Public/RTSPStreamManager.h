// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include <thread>
#include <atomic>
#include "RTSPStreamManager.generated.h"

class UTexture2D;
struct FUpdateTextureRegion2D;

UCLASS()
class RTSPTOVIDEO_API URTSPStreamManager : public UObject
{
    GENERATED_BODY()

public:
    void StartStream(const FString& URL);
    void StopStream();
    TWeakObjectPtr<UTexture2D> GetVideoTexture() const { return VideoTexture; }

    virtual void BeginDestroy() override;
private:
    void InitFFmpeg(const FString& URL); // Initializes FFmpeg context
    //void DecodeLoop();                   // Runs decoding loop on a separate thread
    void UpdateTexture(uint8* FrameData, int Width, int Height); // Copies decoded frame to Texture

    std::thread* DecodeThread;
    std::atomic<bool> bStreaming = false;
    std::atomic<bool> bIsDestroyed = false;

    //UTexture2D* VideoTexture = nullptr; // Texture that is shown in UImage
    TWeakObjectPtr<UTexture2D> VideoTexture;

    FUpdateTextureRegion2D* VideoUpdateRegion = nullptr;
    TArray<uint8> FrameBuffer;

    FCriticalSection ResourceCriticalSection;
};


