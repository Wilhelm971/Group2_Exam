


#include "BuildingPreview.h"
#include "Components/StaticMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"

ABuildingPreview::ABuildingPreview()
{
	PrimaryActorTick.bCanEverTick = false;
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	RootComponent = MeshComp;

	// Allow overlap checks (we'll detect collisions via sweep in controller)
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetCastShadow(false);
}

void ABuildingPreview::BeginPlay()
{
	Super::BeginPlay();

	// If the mesh has a material, create a dynamic material so we can change params at runtime
	if (MeshComp && MeshComp->GetMaterial(0))
	{
		DynamicMat = MeshComp->CreateDynamicMaterialInstance(0);
	}
}

void ABuildingPreview::SetIsValidPlacement(bool bValid)
{
	if (!DynamicMat) return;

	// Expect a scalar param named "Valid" or color param called "Tint" depending on your material.
	// Example: set a color or opacity parameter.
	if (bValid)
	{
		DynamicMat->SetScalarParameterValue(TEXT("Blend"), 0.0f); // your param names may differ
	}
	else
	{
		DynamicMat->SetScalarParameterValue(TEXT("Blend"), 1.0f);
	}
}

