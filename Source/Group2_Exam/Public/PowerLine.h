#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Components/StaticMeshComponent.h"
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
	UStaticMeshComponent* LineMesh;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UStaticMesh* CylinderMeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	UMaterialInterface* BaseEmissiveMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Visual")
	FName EmissiveColorParamName = TEXT("EmissiveColor");

	UFUNCTION(BlueprintCallable, Category = "Line")
	void SetLine(const FVector& Start, const FVector& End, const FColor& Color, float Lifetime = 2.0f);

private:
	UPROPERTY()
	UMaterialInstanceDynamic* DynamicMaterial;

	FTimerHandle DestroyTimer;

	UFUNCTION()
	void SelfDestroy();
};