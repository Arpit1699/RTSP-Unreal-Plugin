#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "RTSPStreamManager.h"
#include "RTSPSubsystem.generated.h"

UCLASS()
class RTSPTOVIDEO_API URTSPSubsystem : public UGameInstanceSubsystem
{
    GENERATED_BODY()

public:
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;
    virtual void Deinitialize() override;

    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RTSP")
    URTSPStreamManager* GetStreamManager() const { return StreamManager; }

    /**
     * Convenience helper callable from Blueprint.
     * Usage: GetRTSPSubsystem(self) → GetStreamManager()
     */
    UFUNCTION(BlueprintCallable, BlueprintPure, Category = "RTSP",
        meta = (WorldContext = "WorldContextObject"))
    static URTSPSubsystem* GetRTSPSubsystem(const UObject* WorldContextObject);

private:
    UPROPERTY()
    URTSPStreamManager* StreamManager = nullptr;
};