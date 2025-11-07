

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "W_HUD.generated.h"

/**
 * 
 */
UCLASS()
class GROUP2_EXAM_API AW_HUD : public UUserWidget
{
	GENERATED_BODY()
	
	public
		UFUNCTION(BlueprintImpementableEvent)
	void UpdateResourses(int32 NewResources);
};
