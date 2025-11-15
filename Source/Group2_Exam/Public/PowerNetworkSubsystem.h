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

private:
	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** Adjacency list for node connections. */
	TMap<APowerNode*, TArray<APowerNode*>> PowerGraph;

	/** List of all registered power nodes. */
	UPROPERTY()
	TArray<APowerNode*> AllNodes;
};