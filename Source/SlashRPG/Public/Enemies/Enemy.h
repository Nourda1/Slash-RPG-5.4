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

	/** <AActor> */
	virtual void Tick(float DeltaTime) override;
	virtual float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;
	virtual void Destroyed() override;
	/** </AActor> */

	/** <IHitInterface> */
	virtual void GetHit_Implementation(const FVector& ImpactPoint, AActor* Hitter) override;
	/** </IHitInterface> */
	
protected:
	/** <AActor> */
	virtual void BeginPlay() override;
	void SpawnSoul();
	/** </AActor> */

	/** <ABaseCharacter> */
	virtual void Die_Implementation() override;
	virtual void Attack () override;
	virtual bool CanAttack() override;
	virtual void HandleDamage(float DamageAmount) override;
	virtual void AttackEnd() override;
	/** </ABaseCharacter> */
	
	UPROPERTY(BlueprintReadOnly, VisibleAnywhere)
	EEnemyState EnemyState = EEnemyState::EES_Patrolling;

	
private:

	/** AI Behaviour */
	void InitializeEnemy();
	void PatrolTimerFinished();
	void CheckCombatTarget();
	void CheckPatrolTarget();
	void HideHealthBar();
	void ShowHealthBar();
	void LoseInterest();
	void StartPatrolling();
	void ChaseTarget();
	bool IsOutsideCombatRadius();
	bool IsOutsideAttackRadius();
	bool IsInsideAttackRadius();
	bool IsChasing();
	bool IsAttacking();
	bool IsDead();
	bool IsEngaged();
	void ClearPatrolTimer();
	void StartAttackTimer();
	void ClearAttackTimer();
	bool InTargetRange(AActor* Target, double Radius);
	void MoveToTarget(AActor* Target);
	AActor* ChoosePatrolTarget();
	void SpawnDefaultWeapon();

	
	UFUNCTION()
	void PawnSeen(APawn* SeenPawn); //Callback for OnPawnSeen in UPawnSensingComponent
	
	/** Components */
	UPROPERTY(VisibleAnywhere)
	UHealthBarComponent * HealthBarWidget;

	UPROPERTY(VisibleAnywhere)
	UPawnSensingComponent* PawnSensing;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class AWeapon> WeaponClass;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	double CombatRadius = 1500.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	double AttackRadius = 300.f;

	/** Navigation */

	UPROPERTY()
	class AAIController* EnemyController;
	
	//Current Patrol Target
	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	AActor* PatrolTarget;

	UPROPERTY(EditInstanceOnly, Category = "AI Navigation")
	TArray<AActor*> PatrolTargets;

	UPROPERTY(EditAnywhere)
	double PatrolRadius = 250.f;

	UPROPERTY(EditAnywhere)
	double AcceptanceRadius = 125.f;

	FTimerHandle PatrolTimer;
	
	UPROPERTY(EditAnywhere, Category = "AI Navigation")
	float PatrolWaitMin = 5.f;
	float PatrolWaitMax = 10.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	float PatrollingSpeed = 175.f;
	
	FTimerHandle AttackTimer;

	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMin = 0.5f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float AttackMax = 1.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float ChasingSpeed = 600.f;
	
	UPROPERTY(EditAnywhere, Category = Combat)
	float DeathLifeSpan = 5.f;

	UPROPERTY(EditAnywhere, Category = Combat)
	TSubclassOf<class ASoul> SoulClass;
};




