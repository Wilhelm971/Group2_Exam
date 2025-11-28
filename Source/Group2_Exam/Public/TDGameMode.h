
#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "Blueprint/UserWidget.h"
#include "TDGameMode.generated.h"

UCLASS()
class GROUP2_EXAM_API ATDGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ATDGameMode();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> VictoryWidgetClass;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "UI")
	TSubclassOf<UUserWidget> LoseWidgetClass;

	UFUNCTION(BlueprintCallable, Category = "Game")
	void Victory();

	UFUNCTION(BlueprintCallable, Category = "Game")
	void Defeat();
};