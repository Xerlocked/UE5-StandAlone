// Fill out your copyright notice in the Description page of Project Settings.


#include "InstantWeapon.h"

#include "ShooterCharacter.h"
#include "Engine/DamageEvents.h"
#include "Kismet/GameplayStatics.h"

AInstantWeapon::AInstantWeapon()
{
	CurrentFiringSpread = 0.0f;
	TracerRoundInterval = 3;
	UE_LOG(LogTemp, Log, TEXT("INSTANT WEAPON"));
}

float AInstantWeapon::GetCurrentSpread() const
{
	return CurrentFiringSpread;
}

void AInstantWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& EndTrace, const FVector& ShootDir,
	int32 RandomSeed, float ReticleSpread)
{
	const FVector MuzzleOrigin = GetMuzzleLocation();

	FVector AdjustedAimDirection  = ShootDir;
	FHitResult ImpactResult = Impact;
	
	if(ImpactResult.bBlockingHit)
	{
		// 크로스헤어에서 히트 위치의 발사 방향 조정
		AdjustedAimDirection = (Impact.ImpactPoint - MuzzleOrigin).GetSafeNormal();

		// 총기 머즐에서 새로운 에임 방향으로 다시 트레이싱
		ImpactResult = WeaponTrace(MuzzleOrigin, MuzzleOrigin + (AdjustedAimDirection * InstantConfig.WeaponRange));
	}
	else
	{
		ImpactResult.ImpactPoint = FVector_NetQuantize(EndTrace);
	}
	

	//
	// /* Adjust direction based on desired crosshair impact point and muzzle location */
	// const FVector AimDir = (Impact.ImpactPoint - MuzzleOrigin).GetSafeNormal();
	//
	// const FVector EndTrace = MuzzleOrigin + (AimDir * InstantConfig.WeaponRange);
	// const FHitResult ImpactResult = WeaponTrace(MuzzleOrigin, EndTrace);

	// if (ImpactResult.bBlockingHit)
	// {
	// 	SpawnImpactEffects(ImpactResult);
	// 	SpawnTrailEffect(ImpactResult.ImpactPoint);
	// }
	// else
	// {
	// 	SpawnTrailEffect(EndTrace);
	// }

	ProcessInstantHitVerify(ImpactResult, AdjustedAimDirection);
}

void AInstantWeapon::ProcessInstantHitVerify(const FHitResult& ImpactResult, const FVector& ShootDirection)
{
	if (ImpactResult.GetActor())
	{
		ProcessPlayerDamage(ImpactResult, ShootDirection);
	}

	if(GetNetMode() != NM_DedicatedServer)
	{
		SimulateHitResult(ImpactResult.ImpactPoint);
	}
}

void AInstantWeapon::ProcessPlayerDamage(const FHitResult& ImpactResult, const FVector& ShootDirection)
{
	float ActualHitDamage = InstantConfig.HitDamage;

	FPointDamageEvent PointDamage;
	PointDamage.HitInfo = ImpactResult;
	PointDamage.ShotDirection = ShootDirection;
	PointDamage.Damage = ActualHitDamage;

	ImpactResult.GetActor()->TakeDamage(PointDamage.Damage, PointDamage, MyPawn->Controller, this);
}

void AInstantWeapon::SimulateHitResult(const FVector& ImpactPoint)
{
	const FVector MuzzleOrigin = GetMuzzleLocation();

	const FVector AimDirection = (ImpactPoint - MuzzleOrigin).GetSafeNormal();

	const FVector EndTrace = MuzzleOrigin + (AimDirection * InstantConfig.WeaponRange);
	const FHitResult Impact = WeaponTrace(MuzzleOrigin, EndTrace);

	if(Impact.bBlockingHit)
	{
		SpawnImpactEffects(Impact);
		SpawnTrailEffect(Impact.ImpactPoint);
	}
	else
	{
		SpawnTrailEffect(Impact.ImpactPoint);
	}
}

void AInstantWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	BulletsShotCount++;
	
	const FVector Origin = GetMuzzleLocation();
	FVector ShootDirection = EndPoint - Origin;

	if(BulletsShotCount % TracerRoundInterval == 0)
	{
		if(TracerFX)
		{
			ShootDirection.Normalize();
			UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, ShootDirection.Rotation());
		}
	}
	
	// if (TrailFX)
	// {
	// 	const FVector Origin = GetMuzzleLocation();
	//
	// 	FVector ShootDir = EndPoint - Origin;
	//
	// 	if (UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, ShootDir))
	// 	{
	// 		TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
	// 	}
	// }
	//
	// const FVector Origin = GetMuzzleLocation();
	// FVector ShootDir = EndPoint - Origin;
	//
	// // Only spawn if a minimum distance is satisfied.
	// if (ShootDir.Size() < MinimumProjectileSpawnDistance)
	// {
	// 	return;
	// }
	//
	// if (BulletsShotCount % TracerRoundInterval == 0)
	// {
	// 	if (TracerFX)
	// 	{
	// 		ShootDir.Normalize();
	// 		UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, ShootDir.Rotation());
	// 	}
	// }
	// else 
	// {
	// 	// Only create trails FX by other players.
	// 	if (GetPawnOwner())
	// 	{
	// 		return;
	// 	}
	//
	// 	if (TrailFX)
	// 	{
	// 		UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
	// 		if (TrailPSC)
	// 		{
	// 			TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
	// 		}
	// 	}
	// }
}

void AInstantWeapon::SpawnImpactEffects(const FHitResult& Impact)
{
	// if (ImpactFX && Impact.bBlockingHit)
	// {
	// 	FHitResult UseImpact = Impact;
	//
	// 	// trace again to find component lost during replication
	// 	if (!Impact.Component.IsValid())
	// 	{
	// 		const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
	// 		const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
	// 		FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
	// 		UseImpact = Hit;
	// 	}
	//
	// 	// FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
	// 	// UParticleSystem* EffectActor = GetWorld()->SpawnActorDeferred<UParticleSystem>(ImpactFX., SpawnTransform);
	// 	// if (EffectActor)
	// 	// {
	// 	// 	EffectActor->SurfaceHit = UseImpact;
	// 	// 	UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
	// 	// }
	// }
}

void AInstantWeapon::FireWeapon()
{
	const int32 RandomSeed = FMath::Rand();
	const float CurrentSpread = GetCurrentSpread();

	const FVector AimDir = GetCameraAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector EndTrace = StartTrace + AimDir * InstantConfig.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, EndTrace, AimDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);
}
