#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "TDGameMode.generated.h"

/**
 * ATDGameMode
 * 
 * Game mode for the tower defense game.
 * Sets default pawn and controller classes; handles victory/defeat UI.
 * 
 * @note Centralizes win/lose logic; call Victory/Defeat from other classes.
 */
UCLASS()
class GROUP2_EXAM_API ATDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/**
	 * Default constructor. Sets pawn and controller classes.
	 */
	ATDGameMode();

	/** Widget class for victory screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	/** Widget class for lose screen. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoseWidgetClass;

	/**
	 * Triggers victory state: Pauses game, shows UI.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	void Victory();

	/**
	 * Triggers defeat state: Pauses game, shows UI.
	 */
	UFUNCTION(BlueprintCallable, Category = "Game")
	void Defeat();
};