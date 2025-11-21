// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#include "PowerNetworkSubsystem.h"
#include "PowerNode.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"

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
    bIsPropagating = false;
    bIsAllowedDeleteLines = false;
}

// =============================================================
// REGISTER NODE
// =============================================================
// Adds a node to the network and rebuilds connections.
void UPowerNetworkSubsystem::RegisterNode(APowerNode* Node)
{
    if (!Node) return;

    AllNodes.AddUnique(Node);
    //RebuildConnections();  // Rebuild graph (optimized for large networks).
    // Rebuilds Connections in PlayerController when the building is placed down, and not in preview mode
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
// Reconstructs the power graph based on node ranges using sphere overlaps for optimization.
void UPowerNetworkSubsystem::RebuildConnections()
{

    // Allows it to Removes all debug lines
    bIsAllowedDeleteLines = true;

    
    PowerGraph.Empty();

    // Filter valid nodes to avoid null checks later.
    TArray<APowerNode*> ValidNodes;
    ValidNodes.Reserve(AllNodes.Num());
    for (APowerNode* Node : AllNodes)
    {
        if (Node)
        {
            ValidNodes.Add(Node);
        }
    }

    if (ValidNodes.IsEmpty()) return;

    for (APowerNode* NodeA : ValidNodes)
    {
        TArray<APowerNode*> Neighbors;

        const FVector LocA = NodeA->GetActorLocation();
        const float Range = NodeA->PowerRange;

        // Perform sphere overlap to find potential neighbors.
        TArray<AActor*> OutActors;
        TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
        ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECC_WorldDynamic));  // Adjust channel if needed (e.g., ECC_WorldDynamic for PowerCannon).

        TArray<AActor*> IgnoreActors;
        IgnoreActors.Add(NodeA);

        UKismetSystemLibrary::SphereOverlapActors(GetWorld(), LocA, Range, ObjectTypes, APowerNode::StaticClass(), IgnoreActors, OutActors);

        // Filter and check exact distance.
        for (AActor* Act : OutActors)
        {
            APowerNode* NodeB = Cast<APowerNode>(Act);
            if (NodeB && FVector::Dist(LocA, NodeB->GetActorLocation()) <= Range)
            {
                Neighbors.Add(NodeB);
                /* Draw persistent yellow line for connections (visible at all times).
                   This lets you see the whole graph, and all the connections to each node.
                   Uncomment the line under to see the whole graph*/
                
                //DrawDebugLine(GetWorld(), LocA, NodeB->GetActorLocation(), FColor::Yellow, true, -1.0f, 0, 1.5f);
            }
        }

        PowerGraph.Add(NodeA, Neighbors);
    }
}

// =============================================================
// PROPAGATE POWER
// =============================================================
// Propagates power from a source node using timed BFS for visualization.
void UPowerNetworkSubsystem::PropagatePower(APowerNode* SourceNode)
{
    if (!SourceNode || !PowerGraph.Contains(SourceNode)) return;
    
    if (bIsPropagating) return;  // Ignore new propagation until current finishes

    if (bIsAllowedDeleteLines)
    {
        // Deletes all debug lines if allowed
        FlushPersistentDebugLines(GetWorld());
        bIsAllowedDeleteLines = false;
    }

    
    bIsPropagating = true;
    
    // Clear previous propagation
    Visited.Reset();
    PropagationQueue.Empty();
    GetWorld()->GetTimerManager().ClearTimer(PropagationTimer);

    // Start propagation from source
    PropagationQueue.Enqueue(TPair<APowerNode*, APowerNode*>(nullptr, SourceNode));
    Visited.Add(SourceNode);

    // Start the timer for step-by-step propagation
    GetWorld()->GetTimerManager().SetTimer(PropagationTimer, this, &UPowerNetworkSubsystem::ProcessPropagationStep, PropagationDelay, true);
}


// =============================================================
// PROCESS PROPAGATION STEP
// =============================================================
// Processes one step of the propagation queue with delay for visualization.
void UPowerNetworkSubsystem::ProcessPropagationStep()
{
   TPair<APowerNode*, APowerNode*> Step;
    if (!PropagationQueue.Dequeue(Step))
    {
        GetWorld()->GetTimerManager().ClearTimer(PropagationTimer);
        bIsPropagating = false;
        return;
    }

    APowerNode* From = Step.Key;
    APowerNode* Current = Step.Value;

    // Activate the node
    Current -> ReceivePower(From);

    // Visualize the power flow if from a previous node

    if (From)
    {
        DrawDebugLine(GetWorld(), From->GetActorLocation(), Current->GetActorLocation(), FColor::Cyan, true, -1.0f, 0, 8.0f);
    }

    // Enqueue neighbors
    for (APowerNode* Neighbor : PowerGraph[Current])
    {
        if (!Visited.Contains(Neighbor))
        {
            Visited.Add(Neighbor);
            PropagationQueue.Enqueue(TPair<APowerNode*, APowerNode*>(Current, Neighbor));
        }
    }

}