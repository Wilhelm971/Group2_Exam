#include "PowerCore.h"
#include "PowerNetworkSubsystem.h"
#include "TimerManager.h"

APowerCore::APowerCore()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 500.0f;  // Override base value
	CurrentHealth = MaxHealth;
}

void APowerCore::BeginPlay()
{
	Super::BeginPlay();

	GetWorld()->GetTimerManager().SetTimer(
		TimerHandle_Pulse,
		this,
		&APowerCore::EmitPowerPulse,
		PulseInterval,
		true
	);
}

void APowerCore::EmitPowerPulse()
{
	if (UWorld* World = GetWorld())
	{
		if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
		{
			UE_LOG(LogTemp, Log, TEXT("Main Base emitting BFS power pulse"));
			Net->PropagatePower(this);
		}
	}
}