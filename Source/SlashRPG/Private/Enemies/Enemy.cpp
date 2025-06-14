// Fill out your copyright notice in the Description page of Project Settings.


#include "Enemies/Enemy.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/CapsuleComponent.h"
#include "SlashRPG/DebugMacros.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Components/AttributeComponent.h"
#include "HUD/HealthBarComponent.h"

// Sets default values
AEnemy::AEnemy()
{
	PrimaryActorTick.bCanEverTick = true;

	GetMesh()->SetCollisionObjectType(ECollisionChannel::ECC_WorldDynamic);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Visibility, ECollisionResponse::ECR_Block);
	GetMesh()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);
	GetMesh()->SetGenerateOverlapEvents(true);
	GetCapsuleComponent()->SetCollisionResponseToChannel(ECollisionChannel::ECC_Camera, ECollisionResponse::ECR_Ignore);

	Attributes = CreateDefaultSubobject<UAttributeComponent>(TEXT("Attributes"));

	HealthBarWidget = CreateDefaultSubobject<UHealthBarComponent>(TEXT("HealthBarWidget"));
	HealthBarWidget->SetupAttachment(GetRootComponent());
	
	
}

void AEnemy::BeginPlay()
{
	Super::BeginPlay();
	
}

void AEnemy::Die()
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && DeathMontage)
	{
		AnimInstance->Montage_Play(DeathMontage);
		const int32 Selection = FMath::RandRange(0, 3);
		FName SectionName = FName();
		switch (Selection)
		{
		case 0:
			SectionName = FName("Death1");
			break;
		case 1:
			SectionName = FName("Death2");
			break;
		case 2:
			SectionName = FName("Death3");
			break;
		case 3:
			SectionName = FName("Death4");
		default:
			break;
		}
		AnimInstance->Montage_JumpToSection(SectionName, DeathMontage);
	}
}


void AEnemy::PlayHitReactMontage(const FName& SectionName)
{
	UAnimInstance* AnimInstance = GetMesh()->GetAnimInstance();
	if (AnimInstance && HitReactMontage)
	{
		AnimInstance->Montage_Play(HitReactMontage);
		AnimInstance->Montage_JumpToSection(SectionName, HitReactMontage);
	}
}

// Called every frame
void AEnemy::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AEnemy::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

void AEnemy::GetHit_Implementation(const FVector& ImpactPoint)
{
	//DRAW_SPHERE_COLOR(ImpactPoint, FColor::Cyan);

	if(Attributes && Attributes->IsAlive())
	{
		DirectionalHitReact(ImpactPoint);
	}
	else
	{
		Die();
	}

	if(HitSound)
	{
		UGameplayStatics::PlaySoundAtLocation(
			this,
			HitSound,
			ImpactPoint
			);
	}
	if (HitParticles && GetWorld())
	{
		UGameplayStatics::SpawnEmitterAtLocation(
			GetWorld(),
			HitParticles,
			ImpactPoint
			);
	}
}

float AEnemy::TakeDamage(float DamageAmount, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	if(Attributes && HealthBarWidget)
	{
		Attributes->ReceiveDamage(DamageAmount);
		HealthBarWidget->SetHealthPercent(Attributes->GetHealthPercent());
	}
	return DamageAmount;
	//return Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
}


void AEnemy::DirectionalHitReact(const FVector& ImpactPoint)
{
	const FVector Forward = GetActorForwardVector();
	//Lowers ImpactPoint to enemies actor location Z
	const FVector ImpactLowered(ImpactPoint.X, ImpactPoint.Y, GetActorLocation().Z);
	const FVector ToHit = (ImpactLowered - GetActorLocation()).GetSafeNormal(); //Gets Vector result and normalizes it to 1

	//Forward  * ToHit = |Forward| |ToHIt| * cos(theta)
	//Forward = 1, ToHit = 1, so Forward * ToHit = cos(theta)
	const double CosTheta =  FVector::DotProduct(Forward,ToHit);
	
	//Takes Inverse cosine of Arc Cos of cos(theta) to get theta
	double Theta = FMath::Acos(CosTheta);

	//converts from radians to degrees
	Theta = FMath::RadiansToDegrees(Theta);

	//If crossproduct points down, theta is negative
	const FVector CrossProduct = FVector::CrossProduct(Forward, ToHit);
	if (CrossProduct.Z < 0)
	{
		Theta *= -1.f;
	}

	FName Section("FromBack"); //sets default section name
	if (Theta >= -45.f && Theta < 45.f)
	{
		Section = FName("FromFront");
	}
	else if (Theta >= -135.f && Theta < -45.f)
	{
		Section = FName("FromLeft");
	}
	else if (Theta >= 45.f && Theta < 135.f)
	{
		Section = FName("FromRight");
	}
	
	PlayHitReactMontage(Section);
	
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),GetActorLocation() + CrossProduct * 100.f, 5.f, FColor::Cyan, 5.f);

	if(GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 5.f, FColor::Green, FString::Printf(TEXT("Theta: %f"),Theta));
	} 
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),GetActorLocation() + Forward * 60.f, 5.f, FColor::Red, 5.f);
	//UKismetSystemLibrary::DrawDebugArrow(this, GetActorLocation(),GetActorLocation() + ToHit * 50, 5.f, FColor::Green, 5.f);
}



