#include "PowerNetworkSubsystem.h"
#include "PowerNode.h"
#include "DrawDebugHelpers.h"

void UPowerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
	PowerGraph.Empty();
	AllNodes.Empty();
}

void UPowerNetworkSubsystem::RegisterNode(APowerNode* Node)
{
	if (!Node) return;

	AllNodes.AddUnique(Node);
	RebuildConnections(); // simple rebuild for now (optimize later)
}

void UPowerNetworkSubsystem::UnregisterNode(APowerNode* Node)
{
	if (!Node) return;

	AllNodes.Remove(Node);
	PowerGraph.Remove(Node);

	for (auto& Pair : PowerGraph)
	{
		Pair.Value.Remove(Node);
	}
}

void UPowerNetworkSubsystem::RebuildConnections()
{
	PowerGraph.Empty();

	for (APowerNode* NodeA : AllNodes)
	{
		if (!NodeA) continue;

		TArray<APowerNode*> Neighbors;

		for (APowerNode* NodeB : AllNodes)
		{
			if (NodeA == NodeB || !NodeB) continue;

			float Distance = FVector::Dist(NodeA->GetActorLocation(), NodeB->GetActorLocation());
			if (Distance <= NodeA->PowerRange)
			{
				Neighbors.Add(NodeB);

				//DrawDebugLine(GetWorld(), NodeA->GetActorLocation(), NodeB->GetActorLocation(), FColor::Yellow, false, 2.0f, 0, 1.5f);
			}
		}

		PowerGraph.Add(NodeA, Neighbors);
	}
}

void UPowerNetworkSubsystem::PropagatePower(APowerNode* SourceNode)
{
	if (!SourceNode || !PowerGraph.Contains(SourceNode)) return;

	TSet<APowerNode*> Visited;
	TQueue<APowerNode*> Queue;

	Queue.Enqueue(SourceNode);
	Visited.Add(SourceNode);

	while (!Queue.IsEmpty())
	{
		APowerNode* Current = nullptr;
		Queue.Dequeue(Current);
		if (!Current) continue;

		Current->ReceivePower(nullptr);

		for (APowerNode* Neighbor : PowerGraph[Current])
		{
			if (!Visited.Contains(Neighbor))
			{
				Visited.Add(Neighbor);
				Queue.Enqueue(Neighbor);

				//Draws visual power line

				DrawDebugLine(GetWorld(), Current->GetActorLocation(), Neighbor->GetActorLocation(), FColor::Cyan, false, 2.0f, 0, 8.0f);
			}
		}
	}
}