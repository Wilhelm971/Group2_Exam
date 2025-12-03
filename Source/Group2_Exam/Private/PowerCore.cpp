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
    bIsPlaced = true;

    // Create mesh component.
    CoreMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("CoreMesh"));
    RootComponent = CoreMesh;
    CoreMesh->SetMobility(EComponentMobility::Movable);
    CoreMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    CoreMesh->SetCollisionObjectType(ECC_WorldDynamic);
    CoreMesh->SetCollisionResponseToAllChannels(ECR_Block);
    CoreMesh->SetGenerateOverlapEvents(true);

    // Load default mesh (cylinder for testing).
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cube.Cube"));
    if (MeshAsset.Succeeded())
    {
        CoreStaticMeshAsset = MeshAsset.Object;
    }

    // Load a default material (optional; set in BP or here).
    // static ConstructorHelpers::FObjectFinder<UMaterialInterface> MaterialAsset(TEXT("/Path/To/Material.Material"));
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
        CoreMesh->SetMaterial(0, CoreMaterial);
    }
}

// =============================================================
// BEGIN PLAY
// =============================================================
// Starts the power pulse timer.
void APowerCore::BeginPlay()
{
	Super::BeginPlay();

	/*
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
			Net->DistributePowerFromCore(this, 100.0f);
		}
	}
}

*/