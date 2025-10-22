

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BuildingPreview.generated.h"

UCLASS()
class GROUP2_EXAM_API ABuildingPreview : public AActor
{
	GENERATED_BODY()
	
public:
	ABuildingPreview();

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UStaticMeshComponent* MeshComp;

	// Material instance used to alter color/transparency
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	UMaterialInstanceDynamic* DynamicMat;

	// Call to set valid/invalid color
	void SetIsValidPlacement(bool bValid);

protected:
	virtual void BeginPlay() override;
};
