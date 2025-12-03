#include "PowerLine.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Materials/MaterialInstanceDynamic.h"

/**
 * @brief Default constructor for APowerLine.
 * 
 * Sets up spline and loads default mesh.
 */
APowerLine::APowerLine()
{
    PrimaryActorTick.bCanEverTick = false;

    SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
    RootComponent = SplineComp;
    SplineComp->SetMobility(EComponentMobility::Movable);

    // Load default cylinder mesh
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshFinder.Succeeded())
    {
        SegmentMesh = MeshFinder.Object;
    }
}

/**
 * @brief Called after components are initialized.
 * 
 * Creates and sets up the spline mesh component with dynamic material.
 */
void APowerLine::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Create single spline mesh component (reused)
    SplineMeshComp = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
    SplineMeshComp->RegisterComponent();
    SplineMeshComp->SetMobility(EComponentMobility::Movable); // Explicitly set to Movable
    SplineMeshComp->AttachToComponent(SplineComp, FAttachmentTransformRules::KeepRelativeTransform);
    SplineMeshComp->SetStaticMesh(SegmentMesh);
    SplineMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);

    if (BaseEmissiveMaterial)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(BaseEmissiveMaterial, this);
        SplineMeshComp->SetMaterial(0, DynamicMaterial);
    }
}

/**
 * @brief Sets the line from start to end with color and optional lifetime.
 * 
 * Configures the spline for a straight line and updates the mesh.
 * 
 * @param Start Starting position.
 * @param End Ending position.
 * @param Color Line color.
 * @param Lifetime Time before destruction (-1 for persistent).
 */
void APowerLine::SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime)
{
    // Clear and set spline points (straight line)
    SplineComp->ClearSplinePoints();
    SplineComp->AddSplinePoint(Start, ESplineCoordinateSpace::World, false);
    SplineComp->AddSplinePoint(End, ESplineCoordinateSpace::World, true);  // Update spline

    // Get computed locations/tangents for the single segment
    FVector StartPos = SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
    FVector StartTangent = SplineComp->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
    FVector EndPos = SplineComp->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
    FVector EndTangent = SplineComp->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

    // Set spline mesh for the segment
    SplineMeshComp->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
    SplineMeshComp->SetStartScale(FVector2D(Thickness, Thickness));  // Uniform thin radius
    SplineMeshComp->SetEndScale(FVector2D(Thickness, Thickness));

    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, Color);
    }

    if (Lifetime > 0.0f)
    {
        GetWorldTimerManager().SetTimer(DestroyTimer, this, &APowerLine::SelfDestroy, Lifetime, false);
    }
}

/**
 * @brief Destroys the actor after its lifetime.
 * 
 * Called by timer if lifetime is set.
 */
void APowerLine::SelfDestroy()
{
    Destroy();
}