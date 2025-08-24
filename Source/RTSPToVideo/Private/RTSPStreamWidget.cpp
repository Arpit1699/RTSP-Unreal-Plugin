#include "RTSPStreamWidget.h"
#include "RTSPStreamManager.h"
#include "TimerManager.h"
#include "Engine/World.h"

void URTSPStreamWidget::NativeConstruct()
{
    Super::NativeConstruct();

}

void URTSPStreamWidget::StartStream()
{
    if (!StreamManager)
    {
        StreamManager = NewObject<URTSPStreamManager>(this);
    }

    if (StreamManager && !StreamURL.IsEmpty())
    {
        StreamManager->StartStream(StreamURL);

        // Refresh image
        UpdateImageTexture();
    }
}

void URTSPStreamWidget::StopStream()
{
    if (StreamManager)
    {
        StreamManager->StopStream();
    }
}

void URTSPStreamWidget::UpdateImageTexture()
{
    if (StreamManager && VideoTarget && StreamManager->GetVideoTexture().IsValid())
    {
        VideoTarget->SetBrushFromTexture(StreamManager->GetVideoTexture().Get(), true);
    }

    // Schedule next update
    GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
        {
            UpdateImageTexture();
        });
}

void URTSPStreamWidget::NativeDestruct()
{
    StopStream();
    Super::NativeDestruct();
}
