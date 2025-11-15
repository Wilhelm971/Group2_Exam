// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#include "PowerNetworkSubsystem.h"
#include "PowerNode.h"
#include "DrawDebugHelpers.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// UPowerNetworkSubsystem: Manages a graph of power nodes and their connections.
// Handles registration, rebuilding connections, and power propagation via BFS.

// =============================================================
// INITIALIZE
// =============================================================
// Initializes the subsystem and clears data structures.
void UPowerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PowerGraph.Empty();
    AllNodes.Empty();
}

// =============================================================
// REGISTER NODE
// =============================================================
// Adds a node to the network and rebuilds connections.
void UPowerNetworkSubsystem::RegisterNode(APowerNode* Node)
{
    if (!Node) return;

    AllNodes.AddUnique(Node);
    RebuildConnections();  // Rebuild graph (optimize in future for large networks).
}

// =============================================================
// UNREGISTER NODE
// =============================================================
// Removes a node from the network and cleans up graph references.
void UPowerNetworkSubsystem::UnregisterNode(APowerNode* Node)
{
    if (!Node) return;

    AllNodes.Remove(Node);
    PowerGraph.Remove(Node);

    // Remove references from other nodes.
    for (auto& Pair : PowerGraph)
    {
        Pair.Value.Remove(Node);
    }
}

// =============================================================
// REBUILD CONNECTIONS
// =============================================================
// Reconstructs the power graph based on node ranges.
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
                // Optional debug: DrawDebugLine(GetWorld(), NodeA->GetActorLocation(), NodeB->GetActorLocation(), FColor::Yellow, false, 2.0f, 0, 1.5f);
            }
        }

        PowerGraph.Add(NodeA, Neighbors);
    }
}

// =============================================================
// PROPAGATE POWER
// =============================================================
// Propagates power from a source node using BFS traversal.
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

                // Debug visualization: Draw power propagation line.
                DrawDebugLine(GetWorld(), Current->GetActorLocation(), Neighbor->GetActorLocation(), FColor::Cyan, false, 2.0f, 0, 8.0f);
            }
        }
    }
}