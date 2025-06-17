// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Characters/CharacterTypes.h"
#include "Characters/BaseCharacter.h"
#include "Enemy.generated.h"


class UHealthBarComponent;
class UPawnSensingComponent;

UCLASS()
class SLASHRPG_API AEnemy : public ABaseCharacter
{
	GENERATED_BODY()

public:
	AEnemy();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	
	virtual void GetHit_Implementation(const FVector& ImpactPoint) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

protected:
	virtual void BeginPlay() override;
	
	virtual void Die() override;

	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();

	UPROPERTY(BlueprintReadOnly)
	EDeathPose DeathPose = EDeathPose::EDP_Alive;

	UFUNCTION()
	void PawnSeen(APawn* SeenPawn);
	

private:
	/**
	 * Components
	 */
	UPROPERTY(BlueprintReadWrite, Category = ActionState, meta=(AllowPrivateAccess= "true" ))
	EEnemyState EnemyLifeState = EEnemyState::EES_Alive;

	UPROPERTY(BlueprintReadWrite, Category = ActionState, meta=(AllowPrivateAccess= "true" ))
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;
	
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent * HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;
	
	/**
	 * Animation Montages
	 */

	UPROPERTY()
	AActor* CombatTarget;

	UPROPERTY(EditAnywhere)
	double CombatRadius = 1500.f;

	UPROPERTY(EditAnywhere)
	double AttackRadius = 200.f;
	

	/**
	 * Navigation
	 */

	UPROPERTY()
	class AAIController* EnemyController;
	
	//Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 250.f;

	FTimerHandle PatrolTimer;
	void PatrolTimerFinished();
	void CheckCombatTarget();
	void CheckPatrolTarget();

	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float WaitMin = 3.f;
	float WaitMax = 7.f;
};
