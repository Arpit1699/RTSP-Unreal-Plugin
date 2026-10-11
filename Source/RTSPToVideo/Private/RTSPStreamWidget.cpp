// RTSPStreamWidget.cpp
// Drives URTSPStreamManager for 20+ simultaneous RTSP streams.
// Texture uploads happen in NativeTick via TickStreams() – no recursive timers.

#include "RTSPStreamWidget.h"
#include "RTSPStreamManager.h"
#include "RTSPSubsystem.h"
#include "Engine/World.h"
#include "Materials/MaterialInstanceDynamic.h"

// ─────────────────────────────────────────────────────────────
//  NativeConstruct
// ─────────────────────────────────────────────────────────────
//void URTSPStreamWidget::NativeConstruct()
//{
//    Super::NativeConstruct();
//
//    BaseRTSPMaterial = LoadObject<UMaterialInterface>(
//        nullptr,
//        TEXT("/RTSPToVideo/Materials/M_RTSPZoomPan.M_RTSPZoomPan")
//    );
//
//    // ── Fetch shared manager from subsystem ──────────────────
//    UGameInstance* GI = GetGameInstance();
//    if (GI)
//    {
//        URTSPSubsystem* Subsystem = GI->GetSubsystem<URTSPSubsystem>();
//        if (Subsystem)
//        {
//            StreamManager = Subsystem->GetStreamManager();
//            UE_LOG(LogTemp, Log, TEXT("RTSP RTSPWidget: Got shared StreamManager from subsystem"));
//        }
//        else
//        {
//            UE_LOG(LogTemp, Error, TEXT("RTSP RTSPWidget: URTSPSubsystem not found"));
//        }
//    }
//    else
//    {
//        UE_LOG(LogTemp, Error, TEXT("RTSP RTSPWidget: GameInstance is null"));
//    }
//}
void URTSPStreamWidget::NativeConstruct()
{
    Super::NativeConstruct();

    // ── FIX: Only fallback-load if BP default didn't assign it ──
    if (!BaseRTSPMaterial)
    {
       /* static ConstructorHelpers::FObjectFinder<UMaterialInterface> MatFinder(
            TEXT("/RTSPToVideo/Materials/M_RTSPZoomPan.M_RTSPZoomPan")
        );

        if (MatFinder.Succeeded())
        {
            BaseRTSPMaterial = MatFinder.Object;
        }*/
    }

    UE_LOG(LogTemp, Warning, TEXT("[RTSP][NativeConstruct] BaseRTSPMaterial = %s"),
        BaseRTSPMaterial ? *BaseRTSPMaterial->GetName() : TEXT("NULL — MID will never be created!"));

    // ── Fetch shared manager from subsystem ──────────────────
    UGameInstance* GI = GetGameInstance();
    if (GI)
    {
        URTSPSubsystem* Subsystem = GI->GetSubsystem<URTSPSubsystem>();
        if (Subsystem)
        {
            StreamManager = Subsystem->GetStreamManager();
            UE_LOG(LogTemp, Warning, TEXT("[RTSP][NativeConstruct] StreamManager acquired OK"));

            // Relay the manager's first-frame event for streams owned by this widget
            if (StreamManager && !FirstFrameHandle.IsValid())
            {
                FirstFrameHandle = StreamManager->OnStreamFirstFrame.AddUObject(
                    this, &URTSPStreamWidget::HandleManagerFirstFrame);
            }
        }
        else
        {
            UE_LOG(LogTemp, Error, TEXT("[RTSP][NativeConstruct] URTSPSubsystem not found"));
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("[RTSP][NativeConstruct] GameInstance is null"));
    }
}
// ─────────────────────────────────────────────────────────────
//  NativeDestruct
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::NativeDestruct()
{
    // Stop only the streams this widget started
    // Other widgets' streams keep running
    StopAllStreams();

    if (StreamManager && FirstFrameHandle.IsValid())
    {
        StreamManager->OnStreamFirstFrame.Remove(FirstFrameHandle);
        FirstFrameHandle.Reset();
    }

    // Just null the reference — subsystem owns the manager
    StreamManager = nullptr;

    Super::NativeDestruct();
}

// ─────────────────────────────────────────────────────────────
//  NativeTick  –  uploads decoded frames & refreshes materials
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
    Super::NativeTick(MyGeometry, InDeltaTime);

    if (!StreamManager) return;

    // Let the manager copy any new decoded frames into UTexture2D objects
    StreamManager->TickStreams();

    // Refresh each slot's UImage widget
    for (FRTSPStreamSlot& Slott : StreamSlots)
    {
        if (Slott.StreamID >= 0)
        {
            UpdateSlotTexture(Slott);
        }
    }
}

// ─────────────────────────────────────────────────────────────
//  StartAllStreams
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::StartAllStreams()
{
    for (int32 i = 0; i < StreamSlots.Num(); ++i)
    {
        StartStream(i);
    }
}

// ─────────────────────────────────────────────────────────────
//  StopAllStreams
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::StopAllStreams()
{
    for (int32 i = 0; i < StreamSlots.Num(); ++i)
    {
        StopStream(i);
    }
}

// ─────────────────────────────────────────────────────────────
//  StartStream  (single slot)
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::StartStream(int32 SlotIndex)
{
    if (!StreamSlots.IsValidIndex(SlotIndex))
    {
        UE_LOG(LogTemp, Warning, TEXT("RTSP RTSPWidget: StartStream – invalid slot index %d"), SlotIndex);
        return;
    }

    FRTSPStreamSlot& Slott = StreamSlots[SlotIndex];

    // Already running
    if (Slott.StreamID >= 0)
    {
        UE_LOG(LogTemp, Warning, TEXT("RTSP RTSPWidget: Slot %d already running (ID=%d)"), SlotIndex, Slott.StreamID);
        return;
    }

    if (Slott.StreamURL.IsEmpty() || !Slott.VideoTarget)
    {
        UE_LOG(LogTemp, Warning, TEXT("RTSP RTSPWidget: Slot %d – missing URL or VideoTarget"), SlotIndex);
        return;
    }

    Slott.StreamID    = StreamManager->StartStream(Slott.StreamURL);
    Slott.MID         = nullptr;  // will be created once texture is ready
    Slott.BoundTexture = nullptr;

    UE_LOG(LogTemp, Log, TEXT("RTSP RTSPWidget: Slot %d started → stream ID %d"), SlotIndex, Slott.StreamID);
}

// ─────────────────────────────────────────────────────────────
//  StopStream  (single slot)  ← this was the crashing call
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::StopStream(int32 SlotIndex)
{
    if (!StreamSlots.IsValidIndex(SlotIndex)) return;

    FRTSPStreamSlot& Slott = StreamSlots[SlotIndex];
    if (Slott.StreamID < 0) return; // not running – nothing to do

    // Stop the decode thread and safely free GPU resources
    StreamManager->StopStream(Slott.StreamID);

    // Clear widget-side state
    Slott.StreamID     = -1;
    Slott.MID          = nullptr;
    Slott.BoundTexture = nullptr;

    // Show nothing on the image widget
    if (Slott.VideoTarget)
    {
        Slott.VideoTarget->SetBrushFromTexture(nullptr);
    }

    UE_LOG(LogTemp, Log, TEXT("RTSP RTSPWidget: Slot %d stopped"), SlotIndex);
}

// ─────────────────────────────────────────────────────────────
//  AddAndStartStream  (runtime addition)
// ─────────────────────────────────────────────────────────────
int32 URTSPStreamWidget::AddAndStartStream(const FString& URL, UImage* VideoTarget)
{
    FRTSPStreamSlot NewSlot;
    NewSlot.StreamURL   = URL;
    NewSlot.VideoTarget = VideoTarget;
    int32 SlotIndex = StreamSlots.Add(NewSlot);

    UE_LOG(LogTemp, Display, TEXT("RTSP AddAndStartStream SlotIndex %d, StreamSlots Array Num %d"), SlotIndex, StreamSlots.Num());
    StartStream(SlotIndex);
    return SlotIndex;
}

// ─────────────────────────────────────────────────────────────
//  HandleManagerFirstFrame  →  relays OnSlotFirstFrame
// ─────────────────────────────────────────────────────────────
void URTSPStreamWidget::HandleManagerFirstFrame(int32 StreamID)
{
    for (int32 i = 0; i < StreamSlots.Num(); ++i)
    {
        if (StreamSlots[i].StreamID == StreamID)
        {
            OnSlotFirstFrame.Broadcast(i, StreamID);
            return;
        }
    }
    // StreamID belongs to another widget sharing the manager — ignore.
}

// ─────────────────────────────────────────────────────────────
//  SetZoom
// ─────────────────────────────────────────────────────────────
//void URTSPStreamWidget::SetZoom(int32 SlotIndex, float NewZoom)
//{
//    if (!StreamSlots.IsValidIndex(SlotIndex)) return;
//    FRTSPStreamSlot& Slott = StreamSlots[SlotIndex];
//    Slott.ZoomFactor = FMath::Clamp(NewZoom, MinZoom, MaxZoom);
//
//    // Reset pan when fully zoomed out
//    if (Slott.ZoomFactor <= 1.01f)
//    {
//        Slott.PanVector = FVector2D::ZeroVector;
//    }
//}
//
//// ─────────────────────────────────────────────────────────────
////  SetPan
//// ─────────────────────────────────────────────────────────────
//void URTSPStreamWidget::SetPan(int32 SlotIndex, FVector2D NewPan)
//{
//    if (!StreamSlots.IsValidIndex(SlotIndex)) return;
//    StreamSlots[SlotIndex].PanVector = NewPan;
//}
//
//// ─────────────────────────────────────────────────────────────
////  UpdateSlotTexture  (called every tick per slot)
//// ─────────────────────────────────────────────────────────────
//void URTSPStreamWidget::UpdateSlotTexture(FRTSPStreamSlot& Slott)
//{
//    if (!Slott.VideoTarget || !StreamManager) return;
//
//    UTexture2D* NewTexture = StreamManager->GetVideoTexture(Slott.StreamID);
//    if (!NewTexture) return; // texture not ready yet
//
//    // ── Create MID once per slot ──────────────────────────────
//    if (!Slott.MID && BaseRTSPMaterial)
//    {
//        Slott.MID = UMaterialInstanceDynamic::Create(BaseRTSPMaterial, this);
//        if (Slott.MID)
//        {
//            Slott.VideoTarget->SetBrushFromMaterial(Slott.MID);
//        }
//    }
//
//    // ── Bind texture to MID (or directly to widget if no material) ──
//    if (NewTexture != Slott.BoundTexture)
//    {
//        Slott.BoundTexture = NewTexture;
//
//        if (Slott.MID)
//        {
//            Slott.MID->SetTextureParameterValue(TEXT("VideoTexture"), NewTexture);
//        }
//        else
//        {
//            // Fallback: no material, set texture directly
//            Slott.VideoTarget->SetBrushFromTexture(NewTexture);
//        }
//    }
//
//    // ── Update zoom / pan every tick ─────────────────────────
//    if (Slott.MID)
//    {
//        float Z = (Slott.ZoomFactor <= 1.01f) ? 1.0f : Slott.ZoomFactor;
//        FVector2D P = (Slott.ZoomFactor <= 1.01f) ? FVector2D::ZeroVector : Slott.PanVector;
//        UE_LOG(LogTemp, Display, TEXT("Zoom & Pan %f,Pan.X %f Pan.Y %f"),Z,P.X,P.Y);
//        Slott.MID->SetScalarParameterValue(TEXT("Zoom"), Z);
//        Slott.MID->SetVectorParameterValue(
//            TEXT("Pan"),
//            FLinearColor(P.X, P.Y, 0.f, 0.f)
//        );
//    }
//}

void URTSPStreamWidget::SetZoom(int32 SlotIndex, float NewZoom)
{
    /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][SetZoom] >>> SlotIndex=%d | Requested=%.4f | SlotsNum=%d"),
        SlotIndex, NewZoom, StreamSlots.Num());*/

    if (!StreamSlots.IsValidIndex(SlotIndex))
    {
        /*UE_LOG(LogTemp, Error, TEXT("[RTSP][SetZoom] INVALID SlotIndex=%d — StreamSlots.Num()=%d — SLOT NOT UPDATED"),
            SlotIndex, StreamSlots.Num());*/
        return;
    }

    FRTSPStreamSlot& Slott = StreamSlots[SlotIndex];
    float OldZoom = Slott.ZoomFactor;
    Slott.ZoomFactor = FMath::Clamp(NewZoom, MinZoom, MaxZoom);

    if (Slott.ZoomFactor <= 1.01f)
    {
        Slott.ZoomFactor = 1.0f;
        Slott.PanVector = FVector2D::ZeroVector;
        /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][SetZoom] Slot=%d | Zoomed out — reset to 1.0, Pan cleared | Was=%.4f"),
            SlotIndex, OldZoom);*/
    }
    else
    {
        /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][SetZoom] Slot=%d | %.4f → %.4f (Clamped from %.4f) | MID=%s | StreamID=%s"),
            SlotIndex,
            OldZoom,
            Slott.ZoomFactor,
            NewZoom,
            Slott.MID ? TEXT("VALID") : TEXT("NULL — MID not created yet!"),
            *Slott.StreamURL);*/
    }
}

void URTSPStreamWidget::SetPan(int32 SlotIndex, FVector2D NewPan)
{
    /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][SetPan] >>> SlotIndex=%d | Pan=%.4f,%.4f | SlotsNum=%d"),
        SlotIndex, NewPan.X, NewPan.Y, StreamSlots.Num());*/

    if (!StreamSlots.IsValidIndex(SlotIndex))
    {
        /*UE_LOG(LogTemp, Error, TEXT("[RTSP][SetPan] INVALID SlotIndex=%d — StreamSlots.Num()=%d — SLOT NOT UPDATED"),
            SlotIndex, StreamSlots.Num());*/
        return;
    }

    FRTSPStreamSlot& Slott = StreamSlots[SlotIndex];
    FVector2D OldPan = Slott.PanVector;
    Slott.PanVector = NewPan;

    /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][SetPan] Slot=%d | Pan %.4f,%.4f → %.4f,%.4f | ZoomFactor=%.4f | MID=%s"),
        SlotIndex,
        OldPan.X, OldPan.Y,
        Slott.PanVector.X, Slott.PanVector.Y,
        Slott.ZoomFactor,
        Slott.MID ? TEXT("VALID") : TEXT("NULL — MID not created yet!"));*/
}

void URTSPStreamWidget::UpdateSlotTexture(FRTSPStreamSlot& Slott)
{
    if (!Slott.VideoTarget)
    {
        /*UE_LOG(LogTemp, Error, TEXT("[RTSP][UpdateSlot] StreamID=%d — VideoTarget is NULL — skipping"),
            Slott.StreamID);*/
        return;
    }
    if (!StreamManager)
    {
       /* UE_LOG(LogTemp, Error, TEXT("[RTSP][UpdateSlot] StreamID=%d — StreamManager is NULL — skipping"),
            Slott.StreamID);*/
        return;
    }

    UTexture2D* NewTexture = StreamManager->GetVideoTexture(Slott.StreamID);
    if (!NewTexture)
    {
        /*UE_LOG(LogTemp, Verbose, TEXT("[RTSP][UpdateSlot] StreamID=%d — Texture not ready yet"),
            Slott.StreamID);*/
        return;
    }

    // ── Create MID once per slot ──────────────────────────────
    if (!Slott.MID)
    {
        if (BaseRTSPMaterial)
        {
            Slott.MID = UMaterialInstanceDynamic::Create(BaseRTSPMaterial, this);
            if (Slott.MID)
            {
                Slott.VideoTarget->SetBrushFromMaterial(Slott.MID);
                /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][UpdateSlot] StreamID=%d — MID CREATED and bound to VideoTarget"),
                    Slott.StreamID);*/
            }
            else
            {
                /*UE_LOG(LogTemp, Error, TEXT("[RTSP][UpdateSlot] StreamID=%d — MID CREATE FAILED — UMaterialInstanceDynamic::Create returned null"),
                    Slott.StreamID);*/
            }
        }
        else
        {
           /* UE_LOG(LogTemp, Error, TEXT("[RTSP][UpdateSlot] StreamID=%d — BaseRTSPMaterial is NULL — cannot create MID. Assign it in the widget BP defaults!"),
                Slott.StreamID);*/
           
            
        }
    }

    // ── Bind texture ──────────────────────────────────────────
    if (NewTexture != Slott.BoundTexture)
    {
        /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][UpdateSlot] StreamID=%d — Texture changed: %s → %s"),
            Slott.StreamID,
            Slott.BoundTexture ? *Slott.BoundTexture->GetName() : TEXT("NULL"),
            *NewTexture->GetName());*/

        Slott.BoundTexture = NewTexture;

        if (Slott.MID)
        {
            Slott.MID->SetTextureParameterValue(TEXT("VideoTexture"), NewTexture);
            /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][UpdateSlot] StreamID=%d — VideoTexture param set on MID"),
                Slott.StreamID);*/
        }
        else
        {
            Slott.VideoTarget->SetBrushFromTexture(NewTexture);
            /*UE_LOG(LogTemp, Warning, TEXT("[RTSP][UpdateSlot] StreamID=%d — No MID — texture set directly on brush (fallback)"),
                Slott.StreamID);*/
        }
    }

    // ── Update zoom / pan every tick ─────────────────────────
    if (Slott.MID)
    {
        float Z = (Slott.ZoomFactor <= 1.01f) ? 1.0f : Slott.ZoomFactor;
        FVector2D P = (Slott.ZoomFactor <= 1.01f) ? FVector2D::ZeroVector : Slott.PanVector;

        Slott.MID->SetScalarParameterValue(TEXT("Zoom"), Z);
        Slott.MID->SetVectorParameterValue(TEXT("Pan"), FLinearColor(P.X, P.Y, 0.f, 0.f));

        // Log only when values are non-default to avoid log spam
        if (Z > 1.01f || !P.IsNearlyZero())
        {
            //UE_LOG(LogTemp, Display, TEXT("[RTSP][UpdateSlot] StreamID=%d — Zoom=%.4f | Pan=%.4f,%.4f — written to MID"),
                //Slott.StreamID, Z, P.X, P.Y);
        }
    }
    else
    {
        //UE_LOG(LogTemp, Error, TEXT("[RTSP][UpdateSlot] StreamID=%d — MID is NULL at zoom/pan write — Zoom=%.4f will NOT apply visually"),
            //Slott.StreamID, Slott.ZoomFactor);
    }
}
