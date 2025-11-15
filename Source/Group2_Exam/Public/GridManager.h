// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ANodeActor;  // Forward declaration

/**
 * FNode
 * 
 * Struct representing a node in the grid for pathfinding.
 * Used internally for A* algorithm.
 */
USTRUCT(BlueprintType)
struct FNode
{
    GENERATED_BODY()

    /** Associated actor for this node. */
    UPROPERTY()
    ANodeActor* NodeActor = nullptr;

    /** Grid index (X, Y). */
    FIntPoint Index = FIntPoint(-1, -1);

    /** Flag if the node is walkable. */
    bool bIsWalkable = true;

    /** Pathfinding costs: G (from start), H (heuristic), F (total). */
    float G = 0.f;
    float H = 0.f;
    float F = 0.f;

    /** Parent node for path reconstruction. */
    TObjectPtr<ANodeActor> Parent = nullptr;

    FNode() = default;
    FNode(ANodeActor* InActor, const FIntPoint& InIdx, bool bWalkable)
        : NodeActor(InActor), Index(InIdx), bIsWalkable(bWalkable) {}
};

/**
 * AGridManager
 * 
 * Manages a 2D grid of nodes for pathfinding and obstacle detection.
 * Provides A* pathfinding and grid utilities.
 */
UCLASS()
class GROUP2_EXAM_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    AGridManager();

protected:
    virtual void BeginPlay() override;

public:
    // =============================================================
    // GRID SETTINGS PROPERTIES
    // =============================================================
    /** Class of nodes to spawn in the grid. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    TSubclassOf<ANodeActor> NodeClass;

    /** Origin position of the grid in world space. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector GridOrigin = FVector::ZeroVector;

    /** Number of cells along X axis. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridSizeX = 20;

    /** Number of cells along Y axis. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridSizeY = 20;

    /** Size of each grid cell. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float CellSize = 200.f;

    /** Flag to draw debug grid lines. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    bool bDrawDebugGrid = true;

    // =============================================================
    // RUNTIME DATA
    // =============================================================
    /** Flat array of grid nodes (indexed as X + Y * GridSizeX). */
    UPROPERTY()
    TArray<ANodeActor*> Grid;

    // =============================================================
    // PATHFINDING FUNCTIONS
    // =============================================================
    /** Finds A* path between two grid indices. */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    TArray<FIntPoint> FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx);

    /** Converts world location to grid index. */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    FIntPoint WorldToGridIndex(const FVector& WorldLocation) const;

    /** Converts grid index to world center position. */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    FVector GridToWorldCenter(const FIntPoint& GridIndex) const;

    // =============================================================
    // GRID MANAGEMENT FUNCTIONS
    // =============================================================
    /** Spawns all nodes in the grid. */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SpawnGrid();

    /** Clears and destroys all grid nodes. */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearGrid();

    /** Marks nodes as non-walkable based on overlaps. */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void MarkObstacles();

private:
    // =============================================================
    // PRIVATE PATHFINDING HELPERS
    // =============================================================
    /** Heuristic cost (Manhattan distance). */
    float Heuristic(const FIntPoint& A, const FIntPoint& B) const;

    /** Reconstructs path from came-from map. */
    TArray<FIntPoint> ReconstructPath(TMap<FIntPoint, FIntPoint>& CameFrom, FIntPoint Current) const;

    /** Checks if index is within grid bounds. */
    bool IsValidIndex(const FIntPoint& Idx) const;

    /** Gets neighboring indices (4-directional). */
    TArray<FIntPoint> GetNeighbors(const FIntPoint& Idx) const;
};