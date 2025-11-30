

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "W_HealthBar.generated.h"

/**
 * 
 */
UCLASS()
class GROUP2_EXAM_API UW_HealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintCallable)
	void SetProgress(float Progress);

	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;
protected:
	virtual void NativeConstruct() override;
	
};
