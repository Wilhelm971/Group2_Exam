#include "W_HealthBar.h"

/**
 * @brief Called when the widget is constructed.
 * 
 * Sets up the initial progress to full (1.0f) if the progress bar exists.
 */
void UW_HealthBar::NativeConstruct()
{
	Super::NativeConstruct();

	if (ProgressBar)
	{
		ProgressBar->SetPercent(1.0f);
	}
}

/**
 * @brief Sets the progress of the health bar.
 * 
 * Clamps the progress value between 0.0 and 1.0 before applying it.
 * 
 * @param Progress The desired progress value (should be between 0.0 and 1.0).
 */
void UW_HealthBar::SetProgress(float Progress)
{
	if (!ProgressBar) return;
	
	float ClampedProgress = FMath::Clamp(Progress, 0.0f, 1.0f);
	ProgressBar->SetPercent(ClampedProgress);
}