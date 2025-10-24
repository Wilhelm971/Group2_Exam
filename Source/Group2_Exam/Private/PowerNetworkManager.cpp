#include "PowerNetworkManager.h"
#include "Building.h"
#include "BaseHQ.h"
#include "Engine/World.h"
#include "TimerManager.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/Engine.h"


APowerNetworkManager::APowerNetworkManager()
{
	PrimaryActorTick.bCanEverTick = false;
	TravelDelayPerEdge = 0.25f;

}


void APowerNetworkManager::BeginPlay()
{
	Super::BeginPlay();
	BuildAdjacencyFromManualConnections();

	// Auto-register all buildings in level optionally
	TArray<AActor*> Found;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ABuilding::StaticClass(), Found);
	for (AActor* A : Found) RegisterBuilding(Cast<ABuilding>(A));

}

void APowerNetworkManager::BuildAdjacencyFromManualConnections()
{
	AdjacencyList.Empty();

	for (const FBuildingConnection& C : ManualConnections)
	{
		if (!C.A || !C.B) continue;
		AdjacencyList.FindOrAdd(C.A).AddUnique(C.B);
		AdjacencyList.FindOrAdd(C.B).AddUnique(C.A);
	}
}

void APowerNetworkManager::RegisterBuilding(ABuilding* Building)
{
	if (!Building) return;
	RegisteredBuildings.AddUnique(Building);
	AdjacencyList.FindOrAdd(Building); // ensure key exists
}

void APowerNetworkManager::UnregisterBuilding(ABuilding* Building)
{
	if (!Building) return;
	RegisteredBuildings.Remove(Building);
	AdjacencyList.Remove(Building);
	for (auto& Pair : AdjacencyList) Pair.Value.Remove(Building);
}

void APowerNetworkManager::AddConnection(ABuilding* A, ABuilding* B)
{
	if (!A || !B || A==B) return;
	AdjacencyList.FindOrAdd(A).AddUnique(B);
	AdjacencyList.FindOrAdd(B).AddUnique(A);
}

void APowerNetworkManager::RemoveConnection(ABuilding* A, ABuilding* B)
{
	if (!A || !B) return;
	if (AdjacencyList.Contains(A)) AdjacencyList[A].Remove(B);
	if (AdjacencyList.Contains(B)) AdjacencyList[B].Remove(A);
}

void APowerNetworkManager::PulsePower(ABuilding* Start, float InitialAmount)
{
	if (!Start) return;
	// Kick off propagation with delays for visualization
	StartPropagationWithDelays(Start, InitialAmount);
}

void APowerNetworkManager::StartPropagationWithDelays(ABuilding* Start, float InitialAmount)
{
	// BFS iteration capturing distance (edges) then schedule deliveries
	TQueue<TPair<ABuilding*, int32>> Queue;
	TSet<ABuilding*> Visited;

	Queue.Enqueue({Start, 0});
	Visited.Add(Start);

	// map to track amount per node (simple split model)
	TMap<ABuilding*, float> Pending;
	Pending.Add(Start, InitialAmount);

	while (!Queue.IsEmpty())
	{
		TPair<ABuilding*, int32> Pair;
		Queue.Dequeue(Pair);
		ABuilding* Node = Pair.Key;
		int32 Depth = Pair.Value;

		float ThisAmount = Pending.Contains(Node) ? Pending[Node] : 0.f;
		// Schedule this node's receive after Depth * TravelDelayPerEdge
		ScheduleDeliver(Node, ThisAmount, Depth * TravelDelayPerEdge);

		// compute forwarding amount (example: keep 50% at node)
		float Forward = ThisAmount * 0.5f;

		if (!AdjacencyList.Contains(Node) || Forward <= 0.f) continue;
		TArray<ABuilding*>& Neighbors = AdjacencyList[Node];
		if (Neighbors.Num() == 0) continue;
		float Each = Forward / Neighbors.Num();

		for (ABuilding* N : Neighbors)
		{
			if (!N) continue;
			Pending.FindOrAdd(N) += Each;
			if (!Visited.Contains(N))
			{
				Visited.Add(N);
				Queue.Enqueue({N, Depth + 1});
			}
		}
	}
}

void APowerNetworkManager::ScheduleDeliver(ABuilding* Target, float Amount, float Delay)
{
	if (!Target || Amount <= 0.f) return;
	FTimerDelegate D = FTimerDelegate::CreateUObject(this, &APowerNetworkManager::DeliverPower, Target, Amount);
	GetWorldTimerManager().SetTimerForNextTick(FTimerDelegate::CreateLambda([this, D, Delay]()
	{
		// schedule with Delay
		FTimerHandle H;
		GetWorldTimerManager().SetTimer(H, D, Delay, false);
	}));
	// Draw a small debug marker at schedule time
	DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 40.f, 6, FColor::Blue, false, Delay + 1.0f);
}

void APowerNetworkManager::DeliverPower(ABuilding* Target, float Amount)
{
	if (!Target) return;
	Target->ReceivePower(Amount);

	// debug draw
	DrawDebugSphere(GetWorld(), Target->GetActorLocation(), 60.f, 8, FColor::Green, false, 1.0f, 0, 3.f);
}

void APowerNetworkManager::DrawNetworkDebug(float Duration)
{
	for (const auto& Pair : AdjacencyList)
	{
		ABuilding* A = Pair.Key;
		if (!A) continue;
		for (ABuilding* B : Pair.Value)
		{
			if (!B) continue;
			DrawDebugLine(GetWorld(), A->GetActorLocation(), B->GetActorLocation(), FColor::Cyan, false, Duration, 0, 3.f);
		}
	}
}


