#include "PowerLine.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Materials/MaterialInstanceDynamic.h"

/**
 * APowerLine: Visualizes lines using splines for power effects.
 */

APowerLine::APowerLine()
{
    PrimaryActorTick.bCanEverTick = false;  // Static visual; no updates needed.

    SplineComp = CreateDefaultSubobject<USplineComponent>(TEXT("SplineComp"));
    RootComponent = SplineComp;
    SplineComp->SetMobility(EComponentMobility::Movable);

    // Load default cylinder mesh via helpers.
    static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
    if (MeshFinder.Succeeded())
    {
        SegmentMesh = MeshFinder.Object;
    }
}

// Applies components post-init.
void APowerLine::PostInitializeComponents()
{
    Super::PostInitializeComponents();

    // Create and attach spline mesh (single segment for straight lines).
    SplineMeshComp = NewObject<USplineMeshComponent>(this, USplineMeshComponent::StaticClass());
    SplineMeshComp->RegisterComponent();
    SplineMeshComp->SetMobility(EComponentMobility::Movable);
    SplineMeshComp->AttachToComponent(SplineComp, FAttachmentTransformRules::KeepRelativeTransform);
    SplineMeshComp->SetStaticMesh(SegmentMesh);
    SplineMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);  // Visual only.

    if (BaseEmissiveMaterial)
    {
        DynamicMaterial = UMaterialInstanceDynamic::Create(BaseEmissiveMaterial, this);
        SplineMeshComp->SetMaterial(0, DynamicMaterial);
    }
}

// Sets up the line between start and end.
void APowerLine::SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime)
{
    // Clear and set spline points for straight line.
    SplineComp->ClearSplinePoints();
    SplineComp->AddSplinePoint(Start, ESplineCoordinateSpace::World, false);
    SplineComp->AddSplinePoint(End, ESplineCoordinateSpace::World, true);

    // Get tangents for smooth mesh.
    FVector StartPos = SplineComp->GetLocationAtSplinePoint(0, ESplineCoordinateSpace::World);
    FVector StartTangent = SplineComp->GetTangentAtSplinePoint(0, ESplineCoordinateSpace::World);
    FVector EndPos = SplineComp->GetLocationAtSplinePoint(1, ESplineCoordinateSpace::World);
    FVector EndTangent = SplineComp->GetTangentAtSplinePoint(1, ESplineCoordinateSpace::World);

    // Set mesh for segment with uniform scale.
    SplineMeshComp->SetStartAndEnd(StartPos, StartTangent, EndPos, EndTangent, true);
    SplineMeshComp->SetStartScale(FVector2D(Thickness, Thickness));
    SplineMeshComp->SetEndScale(FVector2D(Thickness, Thickness));

    if (DynamicMaterial)
    {
        DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, Color);  // Apply color.
    }

    if (Lifetime > 0.0f)
    {
        GetWorldTimerManager().SetTimer(DestroyTimer, this, &APowerLine::SelfDestroy, Lifetime, false);
    }
}

// Auto-destroys after timer.
void APowerLine::SelfDestroy()
{
    Destroy();
}