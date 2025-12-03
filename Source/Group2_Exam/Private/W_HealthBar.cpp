


#include "W_HealthBar.h"

// Sets HealthBar to 100 percent
void UW_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (ProgressBar)
	{
		ProgressBar->SetPercent(1.0f);
	}
}

// Sets the progress bar status, and to keep it updated with the assigned objects healthbar
void UW_HealthBar::SetProgress(float Progress)
{
	if (!ProgressBar) return;
	
	float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
	ProgressBar->SetPercent(ClampedProgress);
}

