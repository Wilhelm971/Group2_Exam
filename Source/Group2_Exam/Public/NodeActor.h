// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

/**
 * ANodeActor
 * 
 * Represents a single cell in the grid for pathfinding.
 * Stores grid index and walkability status.
 */
UCLASS()
class GROUP2_EXAM_API ANodeActor : public AActor
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR
	// =============================================================
	ANodeActor();

	// =============================================================
	// PROPERTIES
	// =============================================================
	/** Grid index (X, Y) of this node. */
	UPROPERTY()
	FIntPoint GridIndex = FIntPoint(-1, -1);

	/** Flag indicating if this node is walkable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalkable = true;
};