

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
	// Initates Functions
	UFUNCTION(BlueprintCallable)
	void SetProgress(float Progress);

	// Reference to the ProgressBar.h
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;
protected:
	virtual void NativeConstruct() override;
	
};
