#include "PowerNetworkSubsystem.h"
#include "PowerNode.h"
#include "PowerCore.h"
#include "DormantPowerCores.h"
#include "DrawDebugHelpers.h"
#include "PowerLine.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "TimerManager.h"

/**
 * UPowerNetworkSubsystem: Manages a graph of power nodes and their connections.
 * Handles registration, rebuilding connections, and power distribution.
 */

// =============================================================
// INITIALIZE
// =============================================================
// Initializes the subsystem and clears data structures.
void UPowerNetworkSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
    Super::Initialize(Collection);
    PowerGraph.Empty();
    AllNodes.Empty();

    // Load BP subclass CDO and copy exposed properties (hack to "use" BP without direct instantiation)
    TSubclassOf<UPowerNetworkSubsystem> BPSubsystemClass = LoadClass<UPowerNetworkSubsystem>(nullptr, TEXT("/Script/Engine.Blueprint'/Game/Blueprints/Buildings/BP_PowerNetworkSubsystem.BP_PowerNetworkSubsystem_C'"), nullptr, LOAD_None, nullptr);
    if (BPSubsystemClass)
    {
        UPowerNetworkSubsystem* BP_CDO = Cast<UPowerNetworkSubsystem>(BPSubsystemClass->GetDefaultObject());
        if (BP_CDO)
        {
            UE_LOG(LogTemp, Warning, TEXT("Copied properties from BP_PowerNetworkSubsystem CDO"));
            this->PowerLineClass = BP_CDO->PowerLineClass;
            // Add copies for any other UPROPERTY() you expose and set in BP
        }
    }
    else
    {
        UE_LOG(LogTemp, Error, TEXT("Failed to load BP_PowerNetworkSubsystem! Check path."));
    }
    
    // Set up global pulse timer (intent: periodic power emission from all cores).
    GetWorld()->GetTimerManager().SetTimer(
        GlobalPulseTimer,
        this,
        &UPowerNetworkSubsystem::PulseAllCores,
        5.0f, // PulseInterval (make UProperty if variable).
        true
    );
}

// =============================================================
// REGISTER NODE
// =============================================================
// Adds a node to the network.
void UPowerNetworkSubsystem::RegisterNode(APowerNode* Node)
{
    if (!Node || AllNodes.Contains(Node)) return;  // Avoid duplicates.

    AllNodes.Add(Node);
    PowerGraph.Add(Node, TArray<APowerNode*>());  // Empty adjacency.

    RebuildConnections();  // Update graph on addition.
}

// =============================================================
// UNREGISTER NODE
// =============================================================
// Removes a node from the network.
void UPowerNetworkSubsystem::UnregisterNode(APowerNode* Node)
{
    if (!Node) return;

    AllNodes.Remove(Node);
    PowerGraph.Remove(Node);

    // Remove references from other nodes' adjacencies.
    for (auto& Pair : PowerGraph)
    {
        Pair.Value.Remove(Node);
    }

    RebuildConnections();  // Update after removal.
}

// =============================================================
// REBUILD CONNECTIONS
// =============================================================
// Rebuilds the graph based on distance.
void UPowerNetworkSubsystem::RebuildConnections()
{
    // Clear existing edges.
    for (auto& Pair : PowerGraph)
    {
        Pair.Value.Empty();
    }

    // Connect nodes within range (O(n^2); optimize for large n if needed).
    for (int32 i = 0; i < AllNodes.Num(); ++i)
    {
        for (int32 j = i + 1; j < AllNodes.Num(); ++j)
        {
            APowerNode* A = AllNodes[i];
            APowerNode* B = AllNodes[j];

            if (!A || !B) continue;

            float Dist = FVector::Dist(A->GetActorLocation(), B->GetActorLocation());
            if (Dist <= A->PowerRange && Dist <= B->PowerRange)  // Bidirectional.
            {
                PowerGraph[A].Add(B);
                PowerGraph[B].Add(A);
            }
        }
    }
}

// =============================================================
// DISTRIBUTE POWER FROM CORE
// =============================================================
// Distributes power from a core to its connected component.
void UPowerNetworkSubsystem::DistributePowerFromCore(APowerCore* SourceCore, float Amount)
{
    if (!SourceCore) return;

    TSet<APowerNode*> Component = GetConnectedComponent(SourceCore);

    for (APowerNode* Node : Component)
    {
        if (Node != SourceCore)  // Skip source.
        {
            Node->ReceivePowerAmount(Amount);  // Distribute evenly; adjust for decay if needed.
        }
    }
}

// =============================================================
// GET CONNECTED COMPONENT
// =============================================================
// Gets the connected subgraph using BFS.
TSet<APowerNode*> UPowerNetworkSubsystem::GetConnectedComponent(APowerNode* StartNode)
{
    TSet<APowerNode*> Component;
    TSet<APowerNode*> VisitedLocal;
    TQueue<APowerNode*> Queue;

    Queue.Enqueue(StartNode);
    VisitedLocal.Add(StartNode);

    while (!Queue.IsEmpty())
    {
        APowerNode* Current;
        Queue.Dequeue(Current);
        Component.Add(Current);

        if (const TArray<APowerNode*>* Neighbors = PowerGraph.Find(Current))
        {
            for (APowerNode* Neighbor : *Neighbors)
            {
                if (!VisitedLocal.Contains(Neighbor))
                {
                    VisitedLocal.Add(Neighbor);
                    Queue.Enqueue(Neighbor);
                }
            }
        }
    }

    return Component;
}

// =============================================================
// PULSE ALL CORES
// =============================================================
// Pulses power from all cores.
void UPowerNetworkSubsystem::PulseAllCores()
{
    TArray<AActor*> CoreActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), CoreActors);

    for (AActor* Actor : CoreActors)
    {
        if (APowerCore* Core = Cast<APowerCore>(Actor))
        {
            DistributePowerFromCore(Core, 100.0f);  // Fixed amount per pulse.
        }
    }

    // Start visualization after pulse.
    StartPowerVisualization();
}

// =============================================================
// START POWER VISUALIZATION
// =============================================================
// Starts BFS visualization from all cores.
void UPowerNetworkSubsystem::StartPowerVisualization()
{
    if (bIsPropagating) return;  // Avoid overlap.
    bIsPropagating = true;

    // Clear previous lines.
    for (APowerLine* Line : ActivePowerLines)
    {
        if (Line) Line->Destroy();
    }
    ActivePowerLines.Empty();

    PropagationQueue.Empty();
    Visited.Empty();

    // Find all cores.
    TArray<AActor*> CoreActors;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), CoreActors);

    // Enqueue cores as sources.
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

    // Start timer if queue not empty.
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

    // Visualize if from a source (draw line).
    if (From && PowerLineClass)
    {
        FVector FromLoc = From->GetActorLocation();
        FVector CurrentLoc = Current->GetActorLocation();
        APowerLine* Line = GetWorld()->SpawnActor<APowerLine>(PowerLineClass, FVector::ZeroVector, FRotator::ZeroRotator);
        if (Line)
        {
            Line->SetLine(FromLoc, CurrentLoc, FColor::Cyan, -1.0f);  // Permanent for visualization.
            ActivePowerLines.Add(Line);
        }
    }

    // Enqueue neighbors.
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