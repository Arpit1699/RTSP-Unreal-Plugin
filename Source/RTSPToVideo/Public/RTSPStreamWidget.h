#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "RTSPStreamWidget.generated.h"

class URTSPStreamManager;
class URTSPSubsystem;

/** Fired once per slot when its stream's first decoded frame is actually
 *  showing on the texture. SlotIndex is the value returned by
 *  AddAndStartStream / the index passed to StartStream. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnRTSPSlotFirstFrame, int32, SlotIndex, int32, StreamID);

// ─────────────────────────────────────────────────────────────
//  One stream slot displayed in the widget
// ─────────────────────────────────────────────────────────────
USTRUCT(BlueprintType)
struct FRTSPStreamSlot
{
    GENERATED_BODY()

    /** The RTSP URL to stream. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    FString StreamURL;

    /** The UImage widget that should show this stream's video. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    UImage* VideoTarget = nullptr;

    /** Runtime: stream ID assigned by the manager (-1 = not started). */
    UPROPERTY(BlueprintReadOnly, Category = "RTSP")
    int32 StreamID = -1;

    /** Runtime: material instance for zoom/pan. */
    UPROPERTY()
    UMaterialInstanceDynamic* MID = nullptr;

    /** Runtime: last texture we bound to the MID (avoids redundant SetTextureParameter). */
    UPROPERTY()
    UTexture2D* BoundTexture = nullptr;

    /** Per-slot zoom (1 = no zoom). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    float ZoomFactor = 1.0f;

    /** Per-slot pan offset (normalised -1..1). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    FVector2D PanVector = FVector2D::ZeroVector;
};

// ─────────────────────────────────────────────────────────────
//  URTSPStreamWidget
// ─────────────────────────────────────────────────────────────
UCLASS()
class RTSPTOVIDEO_API URTSPStreamWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // ── Blueprint-editable stream list ────────────────────────

    /** Populate this array in the Blueprint editor (or at runtime) then call StartAllStreams. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    FRTSPStreamSlot StreamSlot;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    TArray<FRTSPStreamSlot> StreamSlots;

    // ── Blueprint-callable API ────────────────────────────────

    /** Start all slots that have a URL and a VideoTarget assigned. */
    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void StartAllStreams();

    /** Stop all running streams. */
    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void StopAllStreams();

    /**
     * Start a single slot by index.
     * Useful for dynamically adding streams at runtime.
     */
    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void StartStream(int32 SlotIndex);

    /**
     * Stop a single slot by index.
     * THIS is where the original crash occurred – now crash-free.
     */
    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void StopStream(int32 SlotIndex);

    /** Add a new stream slot at runtime and immediately start it. Returns the slot index. */
    UFUNCTION(BlueprintCallable, Category = "RTSP")
    int32 AddAndStartStream(const FString& URL, UImage* VideoTarget);

    /** Broadcast when a slot's first video frame is actually visible on its texture.
     *  Bind to this to defer showing the image widget until video is really there. */
    UPROPERTY(BlueprintAssignable, Category = "RTSP")
    FOnRTSPSlotFirstFrame OnSlotFirstFrame;

    // ── Zoom / Pan helpers ────────────────────────────────────

    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void SetZoom(int32 SlotIndex, float NewZoom);

    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void SetPan(int32 SlotIndex, FVector2D NewPan);

    // ── Zoom limits ───────────────────────────────────────────

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    float MinZoom = 1.0f;

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    float MaxZoom = 4.0f;
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    UMaterialInterface* BaseRTSPMaterial;
protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct()  override;
    virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

private:

    UPROPERTY()
    URTSPStreamManager* StreamManager = nullptr;

    /** Handle for the manager's first-frame delegate binding. */
    FDelegateHandle FirstFrameHandle;

    /** Manager callback — maps StreamID to this widget's slot and relays OnSlotFirstFrame. */
    void HandleManagerFirstFrame(int32 StreamID);

    /** Update one slot's image widget from the latest decoded texture. */
    void UpdateSlotTexture(FRTSPStreamSlot& Slot);
};
