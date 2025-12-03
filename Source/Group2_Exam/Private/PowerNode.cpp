#include "PowerNode.h"
#include "PowerNetworkSubsystem.h"
#include "DormantPowerCores.h"
#include "PowerCore.h"
#include "TDGameMode.h"
#include "Kismet/GameplayStatics.h"

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

// Receives power amount.
void APowerNode::ReceivePowerAmount(float Amount)
{
    CurrentPower += Amount;
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

    UpdateHealthBar();
    
    if (CurrentHealth <= 0.0f)
    {
        if (Cast<APowerCore>(this))
        {
            // Turn core into dormant.
            if (UWorld* World = GetWorld())
            {
                FActorSpawnParameters Params;
                Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

                ADormantPowerCores* NewDormant = World->SpawnActor<ADormantPowerCores>(GetActorLocation(), GetActorRotation(), Params);
                if (NewDormant)
                {
                    UE_LOG(LogTemp, Log, TEXT("PowerCore %s turned into DormantPowerCores"), *GetName());
                }

                // Destroy the core (EndPlay unregisters).
                Destroy();

                // Rebuild connections.
                if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
                {
                    Net->RebuildConnections();
                }

                // Check lose condition.
                TArray<AActor*> ActiveCores;
                UGameplayStatics::GetAllActorsOfClass(World, APowerCore::StaticClass(), ActiveCores);
                
                if (ActiveCores.Num() == 0)
                {
                    UE_LOG(LogTemp, Warning, TEXT("LOSE CONDITION MET: 5 dormant PowerCores!"));
                    // TODO: Notify GameMode or handle loss.
                    if (ATDGameMode* GameMode = Cast<ATDGameMode>(UGameplayStatics::GetGameMode(World)))
                    {
                        UE_LOG(LogTemp, Warning, TEXT("Test 3"));
                        GameMode->Defeat();
                    }
                }
            }
        }
        else
        {
            // Normal nodes (e.g., cannons) just destroy.
            UE_LOG(LogTemp, Log, TEXT("%s DESTROYED!"), *GetName());
            bIsDestroyed = true;
            Destroy();
        }
    }
}
// Updates the Health Progress Bar
void APowerNode::UpdateHealthBar()
{
    if (!HealthBar) return;
    
    float Percent = 0.0f;
    
    Percent = CurrentHealth / MaxHealth;

    HealthBar->SetProgress(Percent);
}