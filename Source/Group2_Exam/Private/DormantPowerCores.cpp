#include "DormantPowerCores.h"
#include "PowerCore.h"
#include "PowerNetworkSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Engine/World.h"
#include "TDGameMode.h"  // For Victory()

// =============================================================
// CLASS DESCRIPTION
// =============================================================
// ADormantPowerCores: Accumulates power from the network.
// When threshold is reached, consumes the power, spawns a PowerCore, and destroys itself.

// =============================================================
// CONSTRUCTOR
// =============================================================
ADormantPowerCores::ADormantPowerCores()
{
    PrimaryActorTick.bCanEverTick = false;

    // Create mesh component.
    DormantMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DormantMesh"));
    RootComponent = DormantMesh;
    DormantMesh->SetMobility(EComponentMobility::Movable);
    DormantMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
    DormantMesh->SetCollisionObjectType(ECC_WorldDynamic);
    DormantMesh->SetCollisionResponseToAllChannels(ECR_Block);
    DormantMesh->SetGenerateOverlapEvents(true);

    // Load default mesh (cylinder for testing).
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshAsset(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshAsset.Succeeded())
    {
        DormantStaticMeshAsset = MeshAsset.Object;
    }
}

// =============================================================
// POST INITIALIZE COMPONENTS
// =============================================================
// Applies default mesh after components are created.
void ADormantPowerCores::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    if (DormantMesh && DormantStaticMeshAsset)
    {
        DormantMesh->SetStaticMesh(DormantStaticMeshAsset);
    }

    if (DormantMesh && DormantMaterial)
    {
        DormantMesh->SetMaterial(0, DormantMaterial);
    }
}

// =============================================================
// BEGIN PLAY
// =============================================================
void ADormantPowerCores::BeginPlay()
{
    Super::BeginPlay();
}

// =============================================================
// RECEIVE POWER AMOUNT
// =============================================================
void ADormantPowerCores::ReceivePowerAmount(float Amount)
{
    AccumulatedPower += Amount;

    if (AccumulatedPower >= ActivationThreshold)
    {
        AccumulatedPower -= ActivationThreshold;
        Activate();
    }
}

// =============================================================
// ACTIVATE
// =============================================================
void ADormantPowerCores::Activate()
{
    if (!GetWorld()) return;

    // Spawn active PowerCore.
    FActorSpawnParameters Params;
    Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

    APowerCore* NewCore = GetWorld()->SpawnActor<APowerCore>(GetActorLocation(), GetActorRotation(), Params);
    if (NewCore)
    {
        UE_LOG(LogTemp, Log, TEXT("DormantPowerCores activated to PowerCore at %s"), *NewCore->GetActorLocation().ToString());
    }

    // Destroy this dormant core (EndPlay will unregister).
    Destroy();

    // Rebuild network connections after change.
    if (UPowerNetworkSubsystem* Net = GetWorld()->GetSubsystem<UPowerNetworkSubsystem>())
    {
        Net->RebuildConnections();
    }

    // Check win condition.
    CheckWinCondition();
}

// =============================================================
// CHECK WIN CONDITION
// =============================================================
void ADormantPowerCores::CheckWinCondition()
{
    TArray<AActor*> ActiveCores;
    UGameplayStatics::GetAllActorsOfClass(GetWorld(), APowerCore::StaticClass(), ActiveCores);

    if (ActiveCores.Num() >= 5)
    {
        UE_LOG(LogTemp, Warning, TEXT("WIN CONDITION MET: 5 active PowerCores!"));
        if (ATDGameMode* GameMode = Cast<ATDGameMode>(UGameplayStatics::GetGameMode(GetWorld())))
        {
            GameMode->Victory();
        }
    }
}