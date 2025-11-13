#pragma once

#include "CoreMinimal.h"
#include "PowerNode.h"
#include "Components/StaticMeshComponent.h"
#include "PowerCannon.generated.h"



UCLASS()
class GROUP2_EXAM_API APowerCannon : public APowerNode
{
	GENERATED_BODY()

public:
	APowerCannon();

protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "PowerCannon")
	int32 TowerCost;

    // Add this property to your APowerCannon class definition
    public:
        UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Cannon")
        int32 PowerCannonCost = 50;

public:

	virtual void PostInitializeComponents() override;
	
	// Called when this cannon is powered and ready to shoot
	virtual void ReceivePower(APowerNode* FromNode) override;

	// Called when this cannon loses power
	virtual void LosePower() override;

	// Try to shoot at nearby enemies
	void TryShoot();

	// How often the cannon fires (if powered)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float FireInterval = 2.0f;

	// How far the cannon can detect and shoot enemies
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float AttackRange = 500.0f;

	// Projectile or damage settings
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Cannon")
	float Damage = 10.0f;

	
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mesh")
	UStaticMesh* CannonStaticMeshAsset;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* NormalMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* PreviewValidMaterial;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Materials")
	UMaterialInterface* PreviewInvalidMaterial;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	bool bIsPreviewMode = false;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	bool bPlacementValid = true;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building", meta = (ClampMin = "50.0"))
	float MinPlacementDistance = 200.0f;


	// NEW FUNCTIONS 
	UFUNCTION(BlueprintCallable, Category = "Building")
	void SetPreviewMode(bool bPreview);

	UFUNCTION(BlueprintCallable, Category = "Building")
	void CheckPlacementValidity();

	UFUNCTION(BlueprintCallable, Category = "Building")
	void UpdatePreviewVisuals();


	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	UStaticMeshComponent* CannonMesh;


private:
	FTimerHandle TimerHandle_Fire;
	void FireAtEnemy(AActor* Target);



	
};
