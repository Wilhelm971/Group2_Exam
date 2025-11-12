#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "W_HUD.generated.h"




UCLASS()
class GROUP2_EXAM_API UW_HUD : public UUserWidget
{
	
    GENERATED_BODY()
	
public:
		UFUNCTION(BlueprintImplementableEvent)
	void UpdateResources(int32 NewResources);

public:
	UFUNCTION(BlueprintImplementableEvent, BlueprintCallable, Category = "HUD")
	void UpdateTowerInfo(int32 TowerCost, FString TowerName);
};
