#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NodeActor.generated.h"

/**
 * ENodeState
 * 
 * Enum for node states in pathfinding visualization.
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
 * ANodeActor
 * 
 * Represents a single cell in the grid for pathfinding.
 * Stores grid index, walkability, state, and handles overlaps/clicks for editing.
 * 
 * @note Used by GridManager; supports dynamic material for state colors.
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
     * Constructor with initializer.
     * @param ObjectInitializer Initializer for subobjects.
     */
    ANodeActor(const FObjectInitializer& ObjectInitializer);

    // =============================================================
    // PROPERTIES
    // =============================================================
    
    /** Grid index (X, Y) of this node. */
    UPROPERTY()
    FIntPoint GridIndex = FIntPoint(-1, -1);

    /** Flag indicating if this node is walkable. Toggleable in editor or runtime. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite)
    bool bIsWalkable = true;
    
    /**
     * Called on construction: Applies initial state and material.
     * @param Transform Construction transform.
     */
    virtual void OnConstruction(const FTransform& Transform) override;

    /**
     * Sets the visual state of the node.
     * @param NewState State to apply (updates color).
     */
    UFUNCTION(BlueprintCallable, Category="A* Node")
    void SetState(ENodeState NewState);

    /**
     * Handles click events for toggling walkability in editor.
     * @param ButtonPressed Key pressed.
     */
    virtual void NotifyActorOnClicked(FKey ButtonPressed) override;

    /**
     * Applies color to dynamic material.
     * @param Color Linear color to set.
     */
    void ApplyColor(const FLinearColor& Color);
    
    /** X coordinate for grid (debug/editor). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
    int32 X = 0;

    /** Y coordinate for grid (debug/editor). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Node")
    int32 Y = 0;

    /** Parent node for path reconstruction. */
    UPROPERTY()
    ANodeActor* Parent = nullptr;

    /** Mesh component for the tile. Read-only in Blueprints. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    UStaticMeshComponent* TileMesh;

    /** Collision box for overlaps. */
    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Components")
    class UBoxComponent* CollisionBox;
    
    /** Current state of the node for visualization. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="A* Node")
    ENodeState CurrentState = ENodeState::Default;

    /** Dynamic material for runtime color changes. */
    UPROPERTY()
    UMaterialInstanceDynamic* DynamicMaterial;

protected:
    /**
     * Handles overlap begin for state changes (e.g., with PowerCannon).
     * @param ColliderComp This component.
     * @param OtherActor Overlapping actor.
     * @param OtherComp Other component.
     * @param OtherBodyIndex Body index.
     * @param bFromSweep From sweep.
     * @param SweepResult Hit result.
     */
    UFUNCTION()
    void OnCollisionOverlap(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

    /**
     * Handles overlap end for state reset.
     * @param ColliderComp This component.
     * @param OtherActor Overlapping actor.
     * @param OtherComp Other component.
     * @param OtherBodyIndex Body index.
     */
    UFUNCTION()
    void OnCollisionEnd(UPrimitiveComponent* ColliderComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
};