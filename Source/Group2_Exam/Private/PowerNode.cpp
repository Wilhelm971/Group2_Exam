// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#include "PowerNode.h"
#include "PowerNetworkSubsystem.h"

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// APowerNode: Base class for power-transmitting nodes.
// Handles registration, power state, and damage.

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values.
APowerNode::APowerNode()
{
    PrimaryActorTick.bCanEverTick = false;
    CurrentHealth = MaxHealth;  // Initialize health to max.
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Registers the node with the power network subsystem.
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

// =============================================================
// END PLAY
// =============================================================
// Unregisters the node when destroyed or game ends.
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

// =============================================================
// POWER MANAGEMENT
// =============================================================
// Receives power and logs the change.
void APowerNode::ReceivePower(APowerNode* FromNode)
{
    if (!bIsPowered)
    {
        bIsPowered = true;
        UE_LOG(LogTemp, Log, TEXT("%s received power"), *GetName());
    }
}

// Loses power and logs the change.
void APowerNode::LosePower()
{
    if (bIsPowered)
    {
        bIsPowered = false;
        UE_LOG(LogTemp, Log, TEXT("%s lost power"), *GetName());
    }
}

// =============================================================
// DAMAGE
// =============================================================
// Applies custom damage and destroys if health reaches zero.
void APowerNode::TakeDamageCustom(float DamageAmount)
{
    if (DamageAmount <= 0.0f) return;

    CurrentHealth -= DamageAmount;

    UE_LOG(LogTemp, Warning, TEXT("%s hit! %.0f dmg → %.0f/%.0f HP"),
           *GetName(), DamageAmount, CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.0f)
    {
        UE_LOG(LogTemp, Log, TEXT("%s DESTROYED!"), *GetName());
        bIsDead = true;
        Destroy();
    }
}

