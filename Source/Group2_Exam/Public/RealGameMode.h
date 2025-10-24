/*

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RealGameMode.generated.h"


UCLASS()
class GROUP2_EXAM_API ARealGameMode : public AGameModeBase
{
	GENERATED_BODY()
public:
	ARealGameMode();

	// Player funds
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category="Economy")
	int32 Funds = 200;

	// Add or remove funds
	UFUNCTION(BlueprintCallable)
	void AddFunds(int32 Amount);

	UFUNCTION(BlueprintCallable)
	bool SpendFunds(int32 Amount);
	
};
*/