/**
* @file TDGameMode.h
 * @brief Header file for the ATDGameMode class, handling game mode logic for victory and defeat.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "TDGameMode.generated.h"

/**
 * @class ATDGameMode
 * @brief Game mode for the tower defense game.
 * 
 * Manages victory and defeat conditions, displaying appropriate UI widgets.
 */
UCLASS()
class GROUP2_EXAM_API ATDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/**
	 * @brief Default constructor for ATDGameMode.
	 * 
	 * Sets up default properties.
	 */
	ATDGameMode();

	/** 
	 * @brief Class of the victory widget to display.
	 * 
	 * Editable in defaults only, read-only in blueprints.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	/** 
	 * @brief Class of the lose widget to display.
	 * 
	 * Editable in defaults only, read-only in blueprints.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoseWidgetClass;

	/**
	 * @brief Triggers the victory condition and displays the victory UI.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	void Victory();

	/**
	 * @brief Triggers the defeat condition and displays the lose UI.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	void Defeat();
};