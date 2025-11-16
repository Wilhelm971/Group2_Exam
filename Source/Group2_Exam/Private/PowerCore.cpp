// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#include "PowerCore.h"
#include "PowerNetworkSubsystem.h"
#include "TimerManager.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// APowerCore: Central power source that pulses power to the network at intervals.
// Extends APowerNode with pulsing behavior.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values, overriding base health.
APowerCore::APowerCore()
{
	PrimaryActorTick.bCanEverTick = false;
	MaxHealth = 500.0f;  // Higher health for core.
	CurrentHealth = MaxHealth;
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Starts the power pulse timer.
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

// =============================================================
// EMIT POWER PULSE
// =============================================================
// Triggers power propagation from this core.
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