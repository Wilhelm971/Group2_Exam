

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyCharacter.generated.h"


class APowerCore;


UCLASS()
class GROUP2_EXAM_API AEnemyCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AEnemyCharacter();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	void TakeDamageFromCannon(float DamageAmount);

private:
	UPROPERTY(EditAnywhere, Category = "Enemy")
	float MoveSpeed = 200.f;

	UPROPERTY(EditAnywhere, Category = "Enemy")
	float MaxHealth = 100.f;

	
	UPROPERTY(EditAnywhere, Category = "Enemy")
	float CurrentHealth = 100.f;


	UPROPERTY()
	APowerCore* TargetBase;
};
