
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
 * Handles node registration, connections, and power propagation.
 */
UCLASS(Blueprintable)
class GROUP2_EXAM_API UPowerNetworkSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// =============================================================
	// OVERRIDES
	// =============================================================
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// =============================================================
	// PUBLIC FUNCTIONS
	// =============================================================
	/** Registers a new node in the network. */
	void RegisterNode(APowerNode* Node);

	/** Unregisters a node from the network. */
	void UnregisterNode(APowerNode* Node);

	/** Rebuilds connections between all registered nodes. */
	void RebuildConnections();

	/** Distributes power from a specific core to its connected buildings. */
	void DistributePowerFromCore(APowerCore* SourceCore, float Amount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	TSubclassOf<APowerLine> PowerLineClass;



private:
	// =============================================================
	// PRIVATE FUNCTIONS
	// =============================================================
	/** Gets the connected component for a starting node. */
	TSet<APowerNode*> GetConnectedComponent(APowerNode* StartNode);

	void PulseAllCores();

	
	void StartPowerVisualization();

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

	bool bIsPropagating = false;

	UPROPERTY()
	TArray<APowerLine*> ActivePowerLines;

	float PowerPerPulse = 4000.0f;




};