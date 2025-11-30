#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PowerNetworkSubsystem.generated.h"

class APowerLine;
class APowerNode;  // Forward declaration
class APowerCore;   // Forward declaration

/**
 * UPowerNetworkSubsystem
 * 
 * World subsystem for managing the power network graph.
 * Handles node registration, connections, power propagation, and visualization.
 * 
 * @note Uses adjacency list for graph; rebuilds on changes.
 * @note Visualization uses BFS for step-by-step line drawing.
 * @see APowerNode for registration.
 */
UCLASS(Blueprintable)
class GROUP2_EXAM_API UPowerNetworkSubsystem : public UWorldSubsystem
{
    GENERATED_BODY()

public:
    // =============================================================
    // OVERRIDES
    // =============================================================
    
    /**
     * Initializes subsystem, clears data, and sets up timers.
     * @param Collection Subsystem collection.
     */
    virtual void Initialize(FSubsystemCollectionBase& Collection) override;

    // =============================================================
    // PUBLIC FUNCTIONS
    // =============================================================
    
    /**
     * Registers a new node in the network graph.
     * @param Node Node to add.
     */
    void RegisterNode(APowerNode* Node);

    /**
     * Unregisters a node from the network graph.
     * @param Node Node to remove.
     */
    void UnregisterNode(APowerNode* Node);

    /**
     * Rebuilds connections between all registered nodes based on distance.
     */
    void RebuildConnections();

    /**
     * Distributes power from a specific core to connected buildings.
     * @param SourceCore Starting core.
     * @param Amount Power to distribute.
     */
    void DistributePowerFromCore(APowerCore* SourceCore, float Amount);

    /** Class for spawning power line visuals. */
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
    TSubclassOf<APowerLine> PowerLineClass;

private:
    // =============================================================
    // PRIVATE FUNCTIONS
    // =============================================================
    
    /**
     * Gets the connected component (subgraph) for a starting node using BFS/DFS.
     * @param StartNode Node to start from.
     * @return Set of connected nodes.
     */
    TSet<APowerNode*> GetConnectedComponent(APowerNode* StartNode);

    /**
     * Pulses all cores globally (timer callback).
     */
    void PulseAllCores();

    /**
     * Starts visualization of power flow from all cores.
     */
    void StartPowerVisualization();

    /**
     * Processes one step of visualization (timer callback).
     */
    void ProcessVisualizationStep();

    // =============================================================
    // PRIVATE DATA
    // =============================================================
    
    /** Adjacency list for node connections. */
    TMap<APowerNode*, TArray<APowerNode*>> PowerGraph;

    /** List of all registered power nodes. */
    UPROPERTY()
    TArray<APowerNode*> AllNodes;

    /** Timer handle for global pulse. */
    FTimerHandle GlobalPulseTimer;

    /** Timer handle for propagation steps. */
    FTimerHandle PropagationTimer;

    /** Queue for pending propagation steps (From, Current). */
    TQueue<TPair<APowerNode*, APowerNode*>> PropagationQueue;

    /** Set of visited nodes during propagation. */
    TSet<APowerNode*> Visited;

    /** Delay between propagation steps for visualization (in seconds). */
    float PropagationDelay = 0.2f;

    /** Flag if propagation is active. */
    bool bIsPropagating = false;

    /** Active lines for current visualization (cleared after). */
    UPROPERTY()
    TArray<APowerLine*> ActivePowerLines;
};