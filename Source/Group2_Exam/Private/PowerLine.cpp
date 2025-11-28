#include "PowerLine.h"
#include "Components/StaticMeshComponent.h"
#include "UObject/ConstructorHelpers.h"
#include "TimerManager.h"
#include "Materials/MaterialInstanceDynamic.h"

APowerLine::APowerLine()
{
	PrimaryActorTick.bCanEverTick = false;
/*
	LineMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LineMesh"));
	RootComponent = LineMesh;
	LineMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	*/
}
/*
void APowerLine::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (CylinderMeshAsset)
	{
		LineMesh->SetStaticMesh(CylinderMeshAsset);
	}
	else
	{
		// Fallback
		static ConstructorHelpers::FObjectFinder<UStaticMesh> MeshFinder(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
		if (MeshFinder.Succeeded())
		{
			LineMesh->SetStaticMesh(MeshFinder.Object);
		}
	}

	if (BaseEmissiveMaterial)
	{
		DynamicMaterial = UMaterialInstanceDynamic::Create(BaseEmissiveMaterial, this);
		LineMesh->SetMaterial(0, DynamicMaterial);
	}
}

void APowerLine::SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime)
{
	FVector Mid = (Start + End) * 0.5f;
	FVector Dir = (End - Start).GetSafeNormal();
	float Length = FVector::Dist(Start, End);

	SetActorLocation(Mid);
	SetActorRotation(Dir.Rotation());

	// Scale: X/Y thin, Z length (Cylinder height ~100uu base)
	LineMesh->SetRelativeScale3D(FVector(0.015f, 0.015f, Length / 100.0f));

	if (DynamicMaterial)
	{
		DynamicMaterial->SetVectorParameterValue(EmissiveColorParamName, Color);
	}

	GetWorldTimerManager().SetTimer(DestroyTimer, this, &APowerLine::SelfDestroy, Lifetime, false);
}

void APowerLine::SelfDestroy()
{
	Destroy();
}

*/