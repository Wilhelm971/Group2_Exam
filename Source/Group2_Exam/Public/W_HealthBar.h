/**
* @file W_HealthBar.h
 * @brief Header file for the UW_HealthBar class, a widget for displaying health progress.
 */

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Components/ProgressBar.h"
#include "W_HealthBar.generated.h"

/**
 * @class UW_HealthBar
 * @brief User widget for displaying a health bar.
 * 
 * Binds to a progress bar component to show health percentage.
 */
UCLASS()
class GROUP2_EXAM_API UW_HealthBar : public UUserWidget
{
	GENERATED_BODY()

public:
	/**
	 * @brief Sets the progress of the health bar.
	 * 
	 * @param Progress The progress value (0.0 to 1.0).
	 */
	UFUNCTION(BlueprintCallable)
	void SetProgress(float Progress);

	/** 
	 * @brief The progress bar component.
	 * 
	 * Bound in the widget blueprint.
	 */
	UPROPERTY(meta = (BindWidget))
	UProgressBar* ProgressBar;
protected:
	/**
	 * @brief Called when the widget is constructed.
	 * 
	 * Sets up initial state.
	 */
	virtual void NativeConstruct() override;
	
};