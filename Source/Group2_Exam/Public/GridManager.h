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
 * Provides A* pathfinding and grid utilities like spawning and marking obstacles.
 * 
 * @note Spawns nodes in BeginPlay; supports debug drawing.
 * @see ANodeActor for individual nodes.
 */
UCLASS()
class GROUP2_EXAM_API AGridManager : public AActor
{
    GENERATED_BODY()

public:
    // =============================================================
    // CONSTRUCTOR AND OVERRIDES
    // =============================================================
    
    /**
     * Default constructor. Sets defaults.
     */
    AGridManager();

protected:
    /**
     * Called when the game starts: Spawns grid and marks obstacles.
     */
    virtual void BeginPlay() override;

public:
    // =============================================================
    // GRID SETTINGS PROPERTIES
    // =============================================================
    
    /** Class of nodes to spawn in the grid. Set in editor. */
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

    /** Size of each grid cell (in units). */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float CellSize = 100.f;  // (truncated in doc; assuming value).

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
    
    /**
     * Finds A* path between two grid indices.
     * @param StartIdx Starting grid index.
     * @param EndIdx Ending grid index.
     * @return Array of indices along the path (empty if no path).
     */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    TArray<FIntPoint> FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx);

    /**
     * Converts world location to grid index.
     * @param WorldLocation World position.
     * @return Grid index (clamped to bounds).
     */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    FIntPoint WorldToGridIndex(const FVector& WorldLocation) const;

    /**
     * Converts grid index to world center position.
     * @param GridIndex Index to convert.
     * @return World position at cell center.
     */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    FVector GridToWorldCenter(const FIntPoint& GridIndex) const;

    // =============================================================
    // GRID MANAGEMENT FUNCTIONS
    // =============================================================
    
    /**
     * Spawns all nodes in the grid.
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SpawnGrid();

    /**
     * Clears and destroys all grid nodes.
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearGrid();

    /**
     * Marks nodes as non-walkable based on overlaps with obstacles.
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void MarkObstacles();

    /**
     * Checks if index is within grid bounds.
     * @param Idx Index to check.
     * @return True if valid.
     */
    bool IsValidIndex(const FIntPoint& Idx) const;

private:
    // =============================================================
    // PRIVATE PATHFINDING HELPERS
    // =============================================================
    
    /**
     * Heuristic cost (Manhattan distance) for A*.
     * @param A Point A.
     * @param B Point B.
     * @return Distance estimate.
     */
    float Heuristic(const FIntPoint& A, const FIntPoint& B) const;

    /**
     * Reconstructs path from came-from map.
     * @param CameFrom Map of parent indices.
     * @param Current Ending index.
     * @return Reversed path array.
     */
    TArray<FIntPoint> ReconstructPath(TMap<FIntPoint, FIntPoint>& CameFrom, FIntPoint Current) const;

    /**
     * Gets neighboring indices (4-directional: up/down/left/right).
     * @param Idx Current index.
     * @return Valid neighbors.
     */
    TArray<FIntPoint> GetNeighbors(const FIntPoint& Idx) const;
};