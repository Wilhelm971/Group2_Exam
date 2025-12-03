#pragma once

#include "CoreMinimal.h"
#include "MyUserWidget.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "MyHealthBar.generated.h"

/*
 * This is an old class, that isn't used anymore. We didn't have time to delete it, so just ignore it.
 */
UCLASS()
class GROUP2_EXAM_API UMyHealthBar : public UMyUserWidget
{
	GENERATED_BODY()

	
public:
	UFUNCTION(BlueprintCallable, Category = "Health")
	void SetHealthPercentage(float Percentage);

protected:
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Health")
	TObjectPtr<UProgressBar> HealthProgressBar;

};
