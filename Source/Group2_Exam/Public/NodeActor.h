/**
 * @file NodeActor.h
 * @brief Header file for the ANodeActor class, representing a single cell in the pathfinding grid.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

/**
 * @enum ENodeState
 * @brief Enumeration of possible states for a grid node in pathfinding.
 * 
 * Used to visualize and manage node status during A* or other algorithms.
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

/**
 * @class ANodeActor
 * @brief Represents a single cell in the grid for pathfinding.
 * 
 * Stores grid index and walkability status. Handles state changes, visuals, and interactions.
 */
UCLASS(BlueprintType)
class GROUP2_EXAM_API ANodeActor : public AActor
{
	GENERATED_BODY()

public:
	// =============================================================
	// CONSTRUCTOR
	// =============================================================
	/**
	 * @brief Constructor for ANodeActor.
	 * 
	 * @param ObjectInitializer The object initializer.
	 */
	ANodeActor(const FObjectInitializer& ObjectInitializer);

	// =============================================================
	// PROPERTIES
	// =============================================================
	/** 
	 * @brief Grid index (X, Y) of this node.
	 */
	UPROPERTY()
	FIntPoint GridIndex = FIntPoint(-1, -1);

	/** 
	 * @brief Flag indicating if this node is walkable.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsWalkable = true;
	
	/**
	 * @brief Called when the actor is constructed.
	 * 
	 * @param Transform The transform of the actor.
	 */
	virtual void OnConstruction(const FTransform& Transform) override;

	/**
	 * @brief Sets the state of the node and updates visuals accordingly.
	 * 
	 * @param NewState The new state to set.
	 */
	UFUNCTION(BlueprintCallable, Category="A* Node")
	void SetState(ENodeState NewState);

	/**
	 * @brief Called when the actor is clicked.
	 * 
	 * @param ButtonPressed The key or button pressed.
	 */
	virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

	/**
	 * @brief Applies a color to the node's material.
	 * 
	 * @param Color The linear color to apply.
	 */
	void ApplyColor(const FLinearColor& Color);
	
	/** 
	 * @brief X coordinate in the grid.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 X = 0;

	/** 
	 * @brief Y coordinate in the grid.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
	int32 Y = 0;

	/** 
	 * @brief Parent node for path reconstruction.
	 */
	UPROPERTY()
	ANodeActor* Parent = nullptr;

	/** 
	 * @brief Static mesh component for the tile visuals.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	UStaticMeshComponent* TileMesh;

	/** 
	 * @brief Box component for collision detection.
	 * 
	 * Visible in the editor but read-only in blueprints.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
	class UBoxComponent* CollisionBox;
	
	/** 
	 * @brief Current state of the node.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="A* Node")
	ENodeState CurrentState = ENodeState::Default;

	/** 
	 * @brief Dynamic material instance for runtime color changes.
	 */
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	protected:
	/**
	 * @brief Called when collision overlap begins.
	 * 
	 * @param ColliderComp The component that collided.
	 * @param OtherActor The other actor involved.
	 * @param OtherComp The other component involved.
	 * @param OtherBodyIndex The body index.
	 * @param bFromSweep Whether from a sweep.
	 * @param SweepResult The sweep result.
	 */
	UFUNCTION()
	void OnCollisionOverlap(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	/**
	 * @brief Called when collision overlap ends.
	 * 
	 * @param ColliderComp The component that collided.
	 * @param OtherActor The other actor involved.
	 * @param OtherComp The other component involved.
	 * @param OtherBodyIndex The body index.
	 */
	UFUNCTION()
	void OnCollisionEnd(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
};