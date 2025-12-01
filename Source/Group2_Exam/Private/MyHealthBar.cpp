


#include "MyHealthBar.h"

void UMyHealthBar::SetHealthPercentage(float Percentage)
{

	if (HealthProgressBar)
	{
		HealthProgressBar->SetPercent(Percentage);
	}
}
//