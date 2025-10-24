

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Building.generated.h"

UCLASS()
class GROUP2_EXAM_API ABuilding : public AActor
{
	GENERATED_BODY()
	
public:	
	ABuilding();
	
	virtual void BeginPlay() override;

	// Whether this building needs power to operate
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Building")
	bool bNeedsPower;

	// Stored power available to this building
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Building")
	float StoredPower;

	// Cost in currency to buy/place
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Building")
	int32 BuildCost;

	// Called by PowerNetworkManager when a pulse arrives
	virtual void ReceivePower(float Amount);

	// Hook for subclasses (cannon) to react immediately
	virtual void OnReceivePower(float Amount);

	// Called every tick (if you enable)
	virtual void Tick(float DeltaTime) override;

};
