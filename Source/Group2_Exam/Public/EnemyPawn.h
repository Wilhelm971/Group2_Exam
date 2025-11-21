// Copyright © 2025 Wilhelm Velde Koren. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "EnemyPawn.generated.h"

class APowerNode;
class AGridManager;

UCLASS()
class GROUP2_EXAM_API AEnemyPawn : public APawn
{
	GENERATED_BODY()

public:
	AEnemyPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// Stats
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MaxHealth = 100.f;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Stats")
	float CurrentHealth;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float MoveSpeed = 300.f;

	// Damage
	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakeDamageCustom(float DamageAmount);

	UFUNCTION(BlueprintCallable, Category = "Damage")
	void TakeDamageFromCannon(float Damage);

	// AI
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "AI")
	APowerNode* TargetNode;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float AttackRange = 150.f;

private:
	void FindClosestTarget();
	FVector GetNextMoveLocation();

	float TimeSinceLastSearch = 0.f;
	static constexpr float SearchInterval = 0.5f;

	// Pathfinding (optional - works with grid)
	TArray<FVector> PathPoints;
	int32 CurrentPathIndex = 0;
	float TimeSincePathRecalc = 0.f;
	UPROPERTY()
	AGridManager* GridMgr;
	
public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "AI")
	float PathRecalcInterval = 1.f;
	void CalculateGridPath();

	// Components
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components")
	class UStaticMeshComponent* MeshComp;

	// Movement
	void MoveToward(FVector Dir, float DeltaTime);
};