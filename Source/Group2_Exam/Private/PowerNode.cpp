#include "PowerNode.h"
#include "PowerNetworkSubsystem.h"

APowerNode::APowerNode()
{
	PrimaryActorTick.bCanEverTick = false;
}

void APowerNode::BeginPlay()
{
	Super::BeginPlay();

	if (UWorld* World = GetWorld())
	{
		if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
		{
			Net->RegisterNode(this);
		}
	}
}

void APowerNode::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (UWorld* World = GetWorld())
	{
		if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
		{
			Net->UnregisterNode(this);
		}
	}

	Super::EndPlay(EndPlayReason);
}

void APowerNode::ReceivePower(APowerNode* FromNode)
{
	if (!bIsPowered)
	{
		bIsPowered = true;
		UE_LOG(LogTemp, Log, TEXT("%s received power"), *GetName());
	}
}

void APowerNode::LosePower()
{
	if (bIsPowered)
	{
		bIsPowered = false;
		UE_LOG(LogTemp, Log, TEXT("%s lost power"), *GetName());
	}
}