#include "PowerNetworkSubsystem.h"
#include "PowerNode.h"
#include "PowerCore.h"
#include "DormantPowerCores.h"
#include "DrawDebugHelpers.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"   // <-- added for GetAllActorsOfClass
#include "TimerManager.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// UPowerNetworkSubsystem: Manages a graph of power nodes and their connections.
// Handles registration, rebuilding connections, and power distribution.

// =============================================================
// INITIALIZE
// =============================================================
// Initializes the subsystem and clears data structures.
void UPowerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PowerGraph.Empty();
    AllNodes.Empty();

    // Set up global pulse timer (assuming PulseInterval = 5.0f, can make UProperty if needed)


        
    GetWorld()->GetTimerManager().SetTimer(
        GlobalPulseTimer,
        this,
        &UPowerNetworkSubsystem::PulseAllCores,
        5.0f, // PulseIntervall
        true
        );
}

// =============================================================
// REGISTER NODE
// =============================================================
// Adds a node to the network.
void UPowerNetworkSubsystem::RegisterNode(APowerNode* Node)
{
    if (!Node) return;

    AllNodes.AddUnique(Node);
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

    // After rebuilding, trigger an immediate pulse to distribute power and visualize
    PulseAllCores();
}


// =============================================================
// PULSE ALL CORES
// =============================================================
void UPowerNetworkSubsystem::PulseAllCores()
{
    if (!bIsPropagating)
    {
        bIsPropagating = true;
        
        TArray<AActor*> CoreActors;
        UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), CoreActors);

        for (AActor* Actor : CoreActors)
        {
            if (APowerCore* Core = Cast<APowerCore>(Actor))
            {
                DistributePowerFromCore(Core, 3000.0f);
            }
        }

        StartPowerVisualization();

        
    }
}



// =============================================================
// DISTRIBUTE POWER FROM CORE
// =============================================================
void UPowerNetworkSubsystem::DistributePowerFromCore(APowerCore* SourceCore, float Amount)
{
    if (!SourceCore || !PowerGraph.Contains(SourceCore)) return;

    // Get the connected component.
    TSet<APowerNode*> Component = GetConnectedComponent(SourceCore);

    // Update power states (set bIsPowered for all in component).
    for (APowerNode* Node : Component)
    {
        Node->bIsPowered = true;
    }

    // Collect buildings (non-cores, including dormants).
    TArray<APowerNode*> Buildings;
    for (APowerNode* Node : Component)
    {
        if (!Cast<APowerCore>(Node))
        {
            Buildings.Add(Node);
        }
    }

    if (Buildings.Num() == 0) return;

    float PerBuilding = Amount / Buildings.Num();

    for (APowerNode* Building : Buildings)
    {
        Building->ReceivePowerAmount(PerBuilding);
    }

    UE_LOG(LogTemp, Log, TEXT("Distributed %.0f power from %s to %d buildings (%.2f each)"), Amount, *SourceCore->GetName(), Buildings.Num(), PerBuilding);
}

// =============================================================
// GET CONNECTED COMPONENT
// =============================================================
TSet<APowerNode*> UPowerNetworkSubsystem::GetConnectedComponent(APowerNode* StartNode)
{
    TSet<APowerNode*> LocalVisited;
    if (!StartNode) return LocalVisited;

    TQueue<APowerNode*> Queue;
    Queue.Enqueue(StartNode);
    LocalVisited.Add(StartNode);

    while (!Queue.IsEmpty())
    {
        APowerNode* Current;
        Queue.Dequeue(Current);

        const TArray<APowerNode*>* Neighbors = PowerGraph.Find(Current);
        if (Neighbors)
        {
            for (APowerNode* Neighbor : *Neighbors)
            {
                if (!LocalVisited.Contains(Neighbor))
                {
                    LocalVisited.Add(Neighbor);
                    Queue.Enqueue(Neighbor);
                }
            }
        }
    }

    return LocalVisited;
}



// =============================================================
// START POWER VISUALIZATION
// =============================================================
void UPowerNetworkSubsystem::StartPowerVisualization()
{
    // Clear old lines
    FlushPersistentDebugLines(GetWorld());

    // Clear previous visualization
    Visited.Reset();
    PropagationQueue.Empty();
    GetWorld()->GetTimerManager().ClearTimer(PropagationTimer);

    // Find all active PowerCores
    TArray<AActor*> CoreActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), CoreActors);

    // Enqueue all sources (cores)
    for (AActor* Actor : CoreActors)
    {
        if (APowerNode* Core = Cast<APowerNode>(Actor))
        {
            if (!Visited.Contains(Core))
            {
                PropagationQueue.Enqueue(TPair<APowerNode*, APowerNode*>(nullptr, Core));
                Visited.Add(Core);
            }
        }
    }

    // Start the timer if there's something to visualize
    if (!PropagationQueue.IsEmpty())
    {
        GetWorld()->GetTimerManager().SetTimer(PropagationTimer, this, &UPowerNetworkSubsystem::ProcessVisualizationStep, PropagationDelay, true);
    }

    
}

// =============================================================
// PROCESS VISUALIZATION STEP
// =============================================================
void UPowerNetworkSubsystem::ProcessVisualizationStep()
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

    // Visualize the power flow if from a previous node
    if (From)
    {
        DrawDebugLine(GetWorld(), From->GetActorLocation(), Current->GetActorLocation(), FColor::Cyan, true, -1.0f, 0, 8.0f);
    }

    // Enqueue neighbors
    if (const TArray<APowerNode*>* Neighbors = PowerGraph.Find(Current))
    {
        for (APowerNode* Neighbor : *Neighbors)
        {
            if (!Visited.Contains(Neighbor))
            {
                Visited.Add(Neighbor);
                PropagationQueue.Enqueue(TPair<APowerNode*, APowerNode*>(Current, Neighbor));
            }
        }
    }
}