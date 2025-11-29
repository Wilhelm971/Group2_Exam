#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/SplineComponent.h"
#include "Components/SplineMeshComponent.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Engine/StaticMesh.h"
#include "PowerLine.generated.h"

UCLASS()
class GROUP2_EXAM_API APowerLine : public AActor
{
	GENERATED_BODY()

public:
	APowerLine();

protected:
	virtual void PostInitializeComponents() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	USplineComponent* SplineComp;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UStaticMesh* SegmentMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UMaterialInterface* BaseEmissiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FName EmissiveColorParamName = TEXT("EmissiveColor");

	UFUNCTION(BlueprintCallable, Category = "Line")
	void SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime = 2.0f);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Line")
	float Thickness = 0.06f;

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	UPROPERTY()
	USplineMeshComponent* SplineMeshComp;

	FTimerHandle DestroyTimer;

	UFUNCTION()
	void SelfDestroy();
};