#include "PowerNode.h"
#include "PowerNetworkSubsystem.h"
#include "DormantPowerCores.h"
#include "PowerCore.h"
#include "TDGameMode.h"
#include "Kismet/GameplayStatics.h"

/**
 * APowerNode: Base class for power-transmitting nodes.
 * Handles registration, power state, and damage.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
// Sets default values.
APowerNode::APowerNode()
{
    PrimaryActorTick.bCanEverTick = false;  // No per-frame needed for base.
    CurrentHealth = MaxHealth;  // Initialize to full health.
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
            Net->RegisterNode(this);  // Add to graph for connections.
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
            Net->UnregisterNode(this);  // Remove from graph to prevent dangling refs.
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

// Receives power amount.
void APowerNode::ReceivePowerAmount(float Amount)
{
    CurrentPower += Amount;  // Accumulate; subclasses may consume or cap.
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
// Applies custom damage and handles destruction/conversion.
void APowerNode::TakeDamageCustom(float DamageAmount)
{
    if (DamageAmount <= 0.0f) return;

    CurrentHealth -= DamageAmount;

    UE_LOG(LogTemp, Warning, TEXT("%s hit! %.0f dmg → %.0f/%.0f HP"),
           *GetName(), DamageAmount, CurrentHealth, MaxHealth);

    if (CurrentHealth <= 0.0f)
    {
        if (Cast<APowerCore>(this))
        {
            // Convert core to dormant on destruction (intent: gameplay progression).
            if (UWorld* World = GetWorld())
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                ADormantPowerCores* NewDormant = World->SpawnActor<ADormantPowerCores>(GetActorLocation(), GetActorRotation(), Params);
                if (NewDormant)
                {
                    UE_LOG(LogTemp, Log, TEXT("PowerCore %s turned into DormantPowerCores"), *GetName());
                }

                // Destroy core; EndPlay handles unregistration.
                Destroy();

                // Rebuild network after change.
                if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
                {
                    Net->RebuildConnections();
                }

                // Check lose condition: No active cores left.
                TArray<AActor*> ActiveCores;
                UGameplayStatics::GetAllActorsOfClass(World, APowerCore::StaticClass(), ActiveCores);
                
                if (ActiveCores.Num() == 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("LOSE CONDITION MET: 5 dormant PowerCores!"));
                    if (ATDGameMode* GameMode = Cast<ATDGameMode>(UGameplayStatics::GetGameMode(World)))
                    {
                        GameMode->Defeat();
                    }
                }
            }
        }
        else
        {
            // Normal nodes destroy without conversion.
            UE_LOG(LogTemp, Log, TEXT("%s DESTROYED!"), *GetName());
            Destroy();
        }
    }
}