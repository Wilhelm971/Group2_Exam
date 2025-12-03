/**
 * @file PowerNetworkSubsystem.h
 * @brief Header file for the UPowerNetworkSubsystem class, managing the power network graph.
 */

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PowerNetworkSubsystem.generated.h"

class APowerLine;
class APowerNode;  // Forward declaration
class APowerCore;   // Forward declaration

/**
 * @class UPowerNetworkSubsystem
 * @brief World subsystem for managing the power network graph.
 * 
 * Handles node registration, connections, and power propagation with visualization.
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
	 * @brief Initializes the subsystem.
	 * 
	 * @param Collection The subsystem collection.
	 */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// =============================================================
	// PUBLIC FUNCTIONS
	// =============================================================
	/**
	 * @brief Registers a new node in the network.
	 * 
	 * @param Node The node to register.
	 */
	void RegisterNode(APowerNode* Node);

	/**
	 * @brief Unregisters a node from the network.
	 * 
	 * @param Node The node to unregister.
	 */
	void UnregisterNode(APowerNode* Node);

	/**
	 * @brief Rebuilds connections between all registered nodes.
	 */
	void RebuildConnections();

	/**
	 * @brief Distributes power from a specific core to its connected buildings.
	 * 
	 * @param SourceCore The source power core.
	 * @param Amount The amount of power to distribute.
	 */
	void DistributePowerFromCore(APowerCore* SourceCore, float Amount);

	/** 
	 * @brief Class of the power line for visualization.
	 * 
	 * Editable in the editor and blueprints.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visualization")
	TSubclassOf<APowerLine> PowerLineClass;



private:
	// =============================================================
	// PRIVATE FUNCTIONS
	// =============================================================
	/**
	 * @brief Gets the connected component for a starting node.
	 * 
	 * @param StartNode The starting node.
	 * @return Set of connected nodes.
	 */
	TSet<APowerNode*> GetConnectedComponent(APowerNode* StartNode);

	/**
	 * @brief Pulses all power cores in the network.
	 */
	void PulseAllCores();

	
	/**
	 * @brief Starts the power visualization process.
	 */
	void StartPowerVisualization();

	/**
	 * @brief Processes a single step in the visualization propagation.
	 */
	void ProcessVisualizationStep();

	
	// =============================================================
	// PRIVATE DATA
	// =============================================================
	/** 
	 * @brief Adjacency list for node connections.
	 */
	TMap<APowerNode*, TArray<APowerNode*>> PowerGraph;

	/** 
	 * @brief List of all registered power nodes.
	 */
	UPROPERTY()
	TArray<APowerNode*> AllNodes;

	/** 
	 * @brief Timer handle for global pulse.
	 */
	FTimerHandle GlobalPulseTimer;

	/** 
	 * @brief Timer handle for propagation steps.
	 */
	FTimerHandle PropagationTimer;

	/** 
	 * @brief Queue for pending propagation steps (From, Current).
	 */
	TQueue<TPair<APowerNode*, APowerNode*>> PropagationQueue;

	/** 
	 * @brief Set of visited nodes during propagation.
	 */
	TSet<APowerNode*> Visited;

	/** 
	 * @brief Delay between propagation steps for visualization (in seconds).
	 */
	float PropagationDelay = 0.2f;

	/** 
	 * @brief Flag indicating if propagation is in progress.
	 */
	bool bIsPropagating = false;

	/** 
	 * @brief List of active power line visualizations.
	 */
	UPROPERTY()
	TArray<APowerLine*> ActivePowerLines;

	/** 
	 * @brief Amount of power per pulse.
	 */
	float PowerPerPulse = 2000.0f;




};