#include "RTSPSubsystem.h"
#include "Engine/GameInstance.h"
#include "Kismet/GameplayStatics.h"

void URTSPSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    StreamManager = NewObject<URTSPStreamManager>(this);
    UE_LOG(LogTemp, Log, TEXT("RTSPSubsystem: Initialized — StreamManager created"));
}

void URTSPSubsystem::Deinitialize()
{
    if (StreamManager)
    {
        StreamManager->StopAllStreams();
        StreamManager = nullptr;
        UE_LOG(LogTemp, Log, TEXT("RTSPSubsystem: All streams stopped on shutdown"));
    }
    Super::Deinitialize();
}

URTSPSubsystem* URTSPSubsystem::GetRTSPSubsystem(const UObject* WorldContextObject)
{
    if (!WorldContextObject) return nullptr;

    UWorld* World = WorldContextObject->GetWorld();
    if (!World) return nullptr;

    UGameInstance* GI = World->GetGameInstance();
    if (!GI) return nullptr;

    return GI->GetSubsystem<URTSPSubsystem>();
}