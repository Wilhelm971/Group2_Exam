


#include "W_HealthBar.h"

void UW_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (ProgressBar)
	{
		ProgressBar->SetPercent(1.0f);
	}
}

void UW_HealthBar::SetProgress(float Progress)
{
	if (!ProgressBar) return;
	
	float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
	ProgressBar->SetPercent(ClampedProgress);
}

