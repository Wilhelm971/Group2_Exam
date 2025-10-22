
#include "TimerManager.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Building.h"
#include "BaseHQ.h"

#include "PowerNetworkManager.h"

// Sets default values
APowerNetworkManager::APowerNetworkManager()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APowerNetworkManager::BeginPlay()
{
	Super::BeginPlay();

	BuildAdjacencyFromManualConnections();
	
	// Register HQ if assigned
	if (HQReference)
	{
		RegisterBuilding(HQReference);
	}

	// Set timer to repeatedly pulse power
	if (PulseInterval > 0.f)
	{
		GetWorldTimerManager().SetTimer(
			PulseTimerHandle,
			this,
			&APowerNetworkManager::PulsePower,
			PulseInterval,
			true
		);
	}
}

void APowerNetworkManager::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void APowerNetworkManager::RegisterBuilding(ABuilding* Building)
{
	if (!Building)
		return;

	RegisteredBuildings.AddUnique(Building);
}

void APowerNetworkManager::UnregisterBuilding(ABuilding* Building)
{
	RegisteredBuildings.Remove(Building);

	for (auto& Pair : AdjacencyList)
	{
		Pair.Value.Remove(Building);
	}

	AdjacencyList.Remove(Building);
}

void APowerNetworkManager::AddConnection(ABuilding* A, ABuilding* B)
{
	if (!A || !B || A == B) return;

	AdjacencyList.FindOrAdd(A).AddUnique(B);
	AdjacencyList.FindOrAdd(B).AddUnique(A);
}

void APowerNetworkManager::RemoveConnection(ABuilding* A, ABuilding* B)
{
	if (!A || !B) return;

	if (AdjacencyList.Contains(A))
		AdjacencyList[A].Remove(B);
	if (AdjacencyList.Contains(B))
		AdjacencyList[B].Remove(A);
}

void APowerNetworkManager::PulsePower()
{
	if (!HQReference)
	{
		UE_LOG(LogTemp, Warning, TEXT("No HQReference assigned to PowerNetworkManager!"));
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("Pulsing power from HQ..."));
	PropagatePower(HQReference, PowerPerPulse);
}

void APowerNetworkManager::BuildAdjacencyFromManualConnections()
{
	AdjacencyList.Empty();

	for (const FBuildingConnection& Conn : ManualConnections)
	{
		if (!Conn.BuildingA || !Conn.BuildingB) continue;

		AdjacencyList.FindOrAdd(Conn.BuildingA).AddUnique(Conn.BuildingB);
		AdjacencyList.FindOrAdd(Conn.BuildingB).AddUnique(Conn.BuildingA);
	}
}

void APowerNetworkManager::PropagatePower(ABuilding* From, float RemainingPower)
{
	if (!From || RemainingPower <= 0.f)
		return;

	// Example: Apply power to building (implement a method on ABuilding like ReceivePower)
	From->ReceivePower(RemainingPower);

	// Visualize pulse
	if (GEngine)
	{
		DrawDebugSphere(GetWorld(), From->GetActorLocation(), 60.f, 8, FColor::Cyan, false, 1.0f);
	}

	// Decrease remaining power as it travels
	float NextPower = RemainingPower * 0.8f; // 20% loss per hop

	// Propagate to connected buildings
	if (AdjacencyList.Contains(From))
	{
		for (ABuilding* Neighbor : AdjacencyList[From])
		{
			if (Neighbor && Neighbor != From)
			{
				DrawDebugLine(GetWorld(),
					From->GetActorLocation(),
					Neighbor->GetActorLocation(),
					FColor::Green, false, 0.5f, 0, 2.f);

				PropagatePower(Neighbor, NextPower);
			}
		}
	}
}




