#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "PowerNetworkSubsystem.generated.h"


class APowerNode;


UCLASS()
class GROUP2_EXAM_API UPowerNetworkSubsystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	// Register or remove nodes from the power network
	void RegisterNode(APowerNode* Node);
	void UnregisterNode(APowerNode* Node);

	// Rebuild the graph (node connections)
	void RebuildConnections();

	// BFS power propagation from the source
	void PropagatePower(APowerNode* SourceNode);

private:
	// Node adjacency list
	TMap<APowerNode*, TArray<APowerNode*>> PowerGraph;

	// All active nodes
	UPROPERTY()
	TArray<APowerNode*> AllNodes;
};
