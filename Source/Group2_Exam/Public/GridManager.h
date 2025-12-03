/**
 * @file GridManager.h
 * @brief Header file for the AGridManager class and FNode struct, managing a 2D grid for pathfinding.
 */

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GridManager.generated.h"

class ANodeActor;  // Forward declaration

/**
 * @struct FNode
 * @brief Struct representing a node in the grid for pathfinding.
 * 
 * Used internally for A* algorithm, storing costs and parent references.
 */
USTRUCT(BlueprintType)
struct FNode
{
    GENERATED_BODY()

    /** 
     * @brief Associated actor for this node.
     */
    UPROPERTY()
    ANodeActor* NodeActor = nullptr;

    /** 
     * @brief Grid index (X, Y).
     */
    FIntPoint Index = FIntPoint(-1, -1);

    /** 
     * @brief Flag if the node is walkable.
     */
    bool bIsWalkable = true;

    /** 
     * @brief Pathfinding costs: G (from start), H (heuristic), F (total).
     */
    float G = 0.f;
    float H = 0.f;
    float F = 0.f;

    /** 
     * @brief Parent node for path reconstruction.
     */
    TObjectPtr<ANodeActor> Parent = nullptr;

    /**
     * @brief Default constructor.
     */
    FNode() = default;
    
    /**
     * @brief Constructor with initialization.
     * 
     * @param InActor The associated node actor.
     * @param InIdx The grid index.
     * @param bWalkable Walkability flag.
     */
    FNode(ANodeActor* InActor, const FIntPoint& InIdx, bool bWalkable)
        : NodeActor(InActor), Index(InIdx), bIsWalkable(bWalkable) {}
};

/**
 * @class AGridManager
 * @brief Manages a 2D grid of nodes for pathfinding and obstacle detection.
 * 
 * Provides A* pathfinding and grid utilities like spawning, clearing, and coordinate conversions.
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
     * @brief Default constructor for AGridManager.
     * 
     * Sets up default grid properties.
     */
    AGridManager();

protected:
    /**
     * @brief Called when the game starts or when spawned.
     * 
     * Spawns the grid if not already done.
     */
    virtual void BeginPlay() override;

public:
    // =============================================================
    // GRID SETTINGS PROPERTIES
    // =============================================================
    /** 
     * @brief Class of nodes to spawn in the grid.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    TSubclassOf<ANodeActor> NodeClass;

    /** 
     * @brief Origin position of the grid in world space.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    FVector GridOrigin = FVector::ZeroVector;

    /** 
     * @brief Number of cells along X axis.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridSizeX = 20;

    /** 
     * @brief Number of cells along Y axis.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    int32 GridSizeY = 20;

    /** 
     * @brief Size of each grid cell.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    float CellSize = 200.f;

    /** 
     * @brief Flag to draw debug grid lines.
     * 
     * Editable in the editor and blueprints.
     */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Grid")
    bool bDrawDebugGrid = true;

    // =============================================================
    // RUNTIME DATA
    // =============================================================
    /** 
     * @brief Flat array of grid nodes (indexed as X + Y * GridSizeX).
     */
    UPROPERTY()
    TArray<ANodeActor*> Grid;

    // =============================================================
    // PATHFINDING FUNCTIONS
    // =============================================================
    /**
     * @brief Finds A* path between two grid indices.
     * 
     * @param StartIdx The starting grid index.
     * @param EndIdx The ending grid index.
     * @return Array of grid indices representing the path.
     */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    TArray<FIntPoint> FindPath(const FIntPoint& StartIdx, const FIntPoint& EndIdx);

    /**
     * @brief Converts world location to grid index.
     * 
     * @param WorldLocation The world position to convert.
     * @return The corresponding grid index.
     */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    FIntPoint WorldToGridIndex(const FVector& WorldLocation) const;

    /**
     * @brief Converts grid index to world center position.
     * 
     * @param GridIndex The grid index to convert.
     * @return The world position at the center of the cell.
     */
    UFUNCTION(BlueprintCallable, Category = "Pathfinding")
    FVector GridToWorldCenter(const FIntPoint& GridIndex) const;

    // =============================================================
    // GRID MANAGEMENT FUNCTIONS
    // =============================================================
    /**
     * @brief Spawns all nodes in the grid.
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void SpawnGrid();

    /**
     * @brief Clears and destroys all grid nodes.
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void ClearGrid();

    /**
     * @brief Marks nodes as non-walkable based on overlaps.
     */
    UFUNCTION(BlueprintCallable, Category = "Grid")
    void MarkObstacles();

    /**
     * @brief Checks if index is within grid bounds.
     * 
     * @param Idx The index to check.
     * @return True if valid, false otherwise.
     */
    bool IsValidIndex(const FIntPoint& Idx) const;

private:
    // =============================================================
    // PRIVATE PATHFINDING HELPERS
    // =============================================================
    /**
     * @brief Heuristic cost (Manhattan distance).
     * 
     * @param A First point.
     * @param B Second point.
     * @return The heuristic value.
     */
    float Heuristic(const FIntPoint& A, const FIntPoint& B) const;

    /**
     * @brief Reconstructs path from came-from map.
     * 
     * @param CameFrom Map of came-from relations.
     * @param Current The current (end) point.
     * @return Array of points in the path.
     */
    TArray<FIntPoint> ReconstructPath(TMap<FIntPoint, FIntPoint>& CameFrom, FIntPoint Current) const;



    /**
     * @brief Gets neighboring indices (4-directional).
     * 
     * @param Idx The center index.
     * @return Array of neighbor indices.
     */
    TArray<FIntPoint> GetNeighbors(const FIntPoint& Idx) const;
};