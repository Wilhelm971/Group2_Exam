#include "PowerCore.h"
#include "PowerNetworkSubsystem.h"
#include "TimerManager.h"

/**
 * APowerCore: Central power source that pulses power to the network at intervals.
 * Extends APowerNode with pulsing behavior.
 */

// =============================================================
// CONSTRUCTOR
// =============================================================
APowerCore::APowerCore()
{
    PrimaryActorTick.bCanEverTick = false;  // No per-frame updates needed.

    MaxHealth = 500.0f;  // Higher health for core durability.
    CurrentHealth = MaxHealth;

    // Create mesh component: Movable for dynamic placement.
    CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
    RootComponent = CoreMesh;
    CoreMesh->SetMobility(EComponentMobility::Movable);
    CoreMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CoreMesh->SetCollisionObjectType(ECC_WorldDynamic);
    CoreMesh->SetCollisionResponseToAllChannels(ECR_Block);
    CoreMesh->SetGenerateOverlapEvents(true);

    // Load default mesh via constructor helpers (cylinder for testing).
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshAsset.Succeeded())
    {
        CoreStaticMeshAsset = MeshAsset.Object;
    }

    // Load default material (optional; override in BP for emissive/glow effects).
    // static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Path/To/CoreMaterial.CoreMaterial"));
    // if (MaterialAsset.Succeeded()) { CoreMaterial = MaterialAsset.Object; }
}

// =============================================================
// POST INITIALIZE COMPONENTS
// =============================================================
// Applies default mesh after components are created.
void APowerCore::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (CoreMesh && CoreStaticMeshAsset)
    {
        CoreMesh->SetStaticMesh(CoreStaticMeshAsset);
    }

    if (CoreMesh && CoreMaterial)
    {
        CoreMesh->SetMaterial(0, CoreMaterial);  // Index 0 for base material.
    }
}

// =============================================================
// BEGIN PLAY
// =============================================================
void APowerCore::BeginPlay()
{
    Super::BeginPlay();

    /*
    // Start pulse timer (intent: periodic power distribution; now handled in subsystem).
    GetWorld()->GetTimerManager().SetTimer(
        TimerHandle_Pulse,
        this,
        &APowerCore::EmitPowerPulse,
        PulseInterval,
        true
    );
    */
}

// =============================================================
// EMIT POWER PULSE
// =============================================================
// Triggers power distribution from this core.
/*
void APowerCore::EmitPowerPulse()
{
    if (UWorld* World = GetWorld())
    {
        if (UPowerNetworkSubsystem* Net = World->GetSubsystem<UPowerNetworkSubsystem>())
        {
            UE_LOG(LogTemp, Log, TEXT("PowerCore %s emitting power pulse"), *GetName());
            Net->DistributePowerFromCore(this, 100.0f);  // Fixed amount; make configurable.
        }
    }
}
*/