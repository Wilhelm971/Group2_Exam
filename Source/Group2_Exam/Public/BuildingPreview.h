/*

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

protected:
	virtual void BeginPlay() override;
public:
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* Mesh;

	// Set preview validity (green/red) in Blueprint or code
	UFUNCTION(BlueprintCallable)
	void SetValidPlacement(bool bValid);
};
*/