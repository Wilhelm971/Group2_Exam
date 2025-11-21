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

UENUM(BlueprintType)
enum class ENodeState : uint8
{
	Default,
	Blocked,
	Open,
	Closed,
	Path,
	Start,
	Target
};

UCLASS(BlueprintType)
class GROUP2_EXAM_API ANodeActor : public AActor
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR
	// =============================================================
	ANodeActor(const FObjectInitializer& ObjectInitializer);

	// =============================================================
	// PROPERTIES
	// =============================================================
	/** Grid index (X, Y) of this node. */
	UPROPERTY()
	FIntPoint GridIndex = FIntPoint(-1, -1);

	/** Flag indicating if this node is walkable. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalkable = true;
	
	virtual void OnConstruction(const FTransform& Transform) override;

	UFUNCTION(BlueprintCallable, Category="A* Node")
	void SetState(ENodeState NewState);

	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	void ApplyColor(const FLinearColor& Color);
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 X = 0;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 Y = 0;

	UPROPERTY()
	ANodeActor* Parent = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* TileMesh;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UBoxComponent* CollisionBox;
	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="A* Node")
	ENodeState CurrentState = ENodeState::Default;

	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	protected:
	UFUNCTION()
	void OnCollisionOverlap(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
	void OnCollisionEnd(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};