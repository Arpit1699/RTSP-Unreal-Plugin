#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/Image.h"
#include "RTSPStreamWidget.generated.h"

class URTSPStreamManager;

UCLASS()
class RTSPTOVIDEO_API URTSPStreamWidget : public UUserWidget
{
    GENERATED_BODY()

public:
    // Blueprint exposed controls
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    FString StreamURL;

    /*UPROPERTY(meta = (BindWidget))
    UImage* VideoTarget;*/

    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RTSP")
    UImage* VideoTarget;

    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void StartStream();

    UFUNCTION(BlueprintCallable, Category = "RTSP")
    void StopStream();

protected:
    virtual void NativeConstruct() override;
    virtual void NativeDestruct() override;

private:
    UPROPERTY()
    URTSPStreamManager* StreamManager;

    void UpdateImageTexture();
};
