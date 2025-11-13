#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

UCLASS()
class GROUP2_EXAM_API ANodeActor : public AActor
{
	GENERATED_BODY()

public:
	ANodeActor();

	UPROPERTY()
	FIntPoint GridIndex = FIntPoint(-1, -1);

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalkable = true;
};