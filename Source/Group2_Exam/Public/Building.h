

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UCLASS()
class GROUP2_EXAM_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABuilding();
	
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called when network delivers power to this building
	virtual void OnReceivePower(float amount);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	bool bNeedsPower;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	float StoredPower;
	
	virtual void ApplyPower(float amount);
};
