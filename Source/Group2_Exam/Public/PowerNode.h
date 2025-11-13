#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PowerNode.generated.h"

UCLASS()
class GROUP2_EXAM_API APowerNode : public AActor
{
	GENERATED_BODY()
	
public:
	APowerNode();

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

public:
	// Power range (how far this node can send power)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Power")
	float PowerRange = 400.0f;

	// Whether this node is currently powered
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Power")
	bool bIsPowered = false;

	// Called when this node receives power
	virtual void ReceivePower(APowerNode* FromNode);

	// Called when this node loses power
	virtual void LosePower();


	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.0f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UFUNCTION(BlueprintCallable, Category = "Damage")
	virtual void TakeDamageCustom(float DamageAmount);
};
