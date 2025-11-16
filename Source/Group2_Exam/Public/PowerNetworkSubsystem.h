// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.


#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PowerNetworkSubsystem.generated.h"

class APowerNode;  // Forward declaration

/**
 * UPowerNetworkSubsystem
 * 
 * World subsystem for managing the power network graph.
 * Handles node registration, connections, and power propagation.
 */
UCLASS()
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

	/** Propagates power from the source node using BFS. */
	void PropagatePower(APowerNode* SourceNode);

	// Switch to turn on or off the graph
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Graph")
	bool bShowGraph;


	

private:
	// =============================================================
	// PRIVATE FUNCTIONS
	// =============================================================
	/** Processes one step of the propagation with delay for visualization. */
	void ProcessPropagationStep();
	
	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** Adjacency list for node connections. */
	TMap<APowerNode*, TArray<APowerNode*>> PowerGraph;

	/** List of all registered power nodes. */
	UPROPERTY()
	TArray<APowerNode*> AllNodes;

	/** Timer handle for propagation steps. */
	FTimerHandle PropagationTimer;

	/** Queue for pending propagation steps (From, Current). */
	TQueue<TPair<APowerNode*, APowerNode*>> PropagationQueue;

	/** Set of visited nodes during propagation. */
	TSet<APowerNode*> Visited;

	/** Delay between propagation steps for visualization (in seconds). */
	float PropagationDelay = 0.2f;

	/** Flag indicating if propagation is currently in progress. */
	bool bIsPropagating = false;

	// check if it should Redraw the connections
	bool bIsAllowedDeleteLines = false;
};