// Fill out your copyright notice in the Description page of Project Settings.


#include "InstantWeapon.h"

#include "Kismet/GameplayStatics.h"
#include "Particles/ParticleSystemComponent.h"

AInstantWeapon::AInstantWeapon()
{
	CurrentFiringSpread = 0.0f;
}

float AInstantWeapon::GetCurrentSpread() const
{
	return CurrentFiringSpread;
}

void AInstantWeapon::ProcessInstantHit(const FHitResult& Impact, const FVector& Origin, const FVector& ShootDir,
	int32 RandomSeed, float ReticleSpread)
{
	const FVector MuzzleOrigin = GetMuzzleLocation();

	/* Adjust direction based on desired crosshair impact point and muzzle location */
	const FVector AimDir = (Impact.ImpactPoint - MuzzleOrigin).GetSafeNormal();
	
	const FVector EndTrace = MuzzleOrigin + (AimDir * InstantConfig.WeaponRange);
	const FHitResult ImpactResult = WeaponTrace(MuzzleOrigin, EndTrace);

	if (ImpactResult.bBlockingHit)
	{
		SpawnImpactEffects(ImpactResult);
		SpawnTrailEffect(ImpactResult.ImpactPoint);
	}
	else
	{
		SpawnTrailEffect(EndTrace);
	}
}

void AInstantWeapon::SpawnTrailEffect(const FVector& EndPoint)
{
	if (TrailFX)
	{
		const FVector Origin = GetMuzzleLocation();

		FVector ShootDir = EndPoint - Origin;

		if (UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, ShootDir))
		{
			TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
		}
	}

	const FVector Origin = GetMuzzleLocation();
	FVector ShootDir = EndPoint - Origin;

	// Only spawn if a minimum distance is satisfied.
	if (ShootDir.Size() < MinimumProjectileSpawnDistance)
	{
		return;
	}

	if (BulletsShotCount % TracerRoundInterval == 0)
	{
		if (TracerFX)
		{
			ShootDir.Normalize();
			UGameplayStatics::SpawnEmitterAtLocation(this, TracerFX, Origin, ShootDir.Rotation());
		}
	}
	else 
	{
		// Only create trails FX by other players.
		if (GetPawnOwner())
		{
			return;
		}

		if (TrailFX)
		{
			UParticleSystemComponent* TrailPSC = UGameplayStatics::SpawnEmitterAtLocation(this, TrailFX, Origin);
			if (TrailPSC)
			{
				TrailPSC->SetVectorParameter(TrailTargetParam, EndPoint);
			}
		}
	}
}

void AInstantWeapon::SpawnImpactEffects(const FHitResult& Impact)
{
	if (ImpactFX && Impact.bBlockingHit)
	{
		FHitResult UseImpact = Impact;

		// trace again to find component lost during replication
		if (!Impact.Component.IsValid())
		{
			const FVector StartTrace = Impact.ImpactPoint + Impact.ImpactNormal * 10.0f;
			const FVector EndTrace = Impact.ImpactPoint - Impact.ImpactNormal * 10.0f;
			FHitResult Hit = WeaponTrace(StartTrace, EndTrace);
			UseImpact = Hit;
		}

		// FTransform const SpawnTransform(Impact.ImpactNormal.Rotation(), Impact.ImpactPoint);
		// UParticleSystem* EffectActor = GetWorld()->SpawnActorDeferred<UParticleSystem>(ImpactFX., SpawnTransform);
		// if (EffectActor)
		// {
		// 	EffectActor->SurfaceHit = UseImpact;
		// 	UGameplayStatics::FinishSpawningActor(EffectActor, SpawnTransform);
		// }
	}
}

void AInstantWeapon::FireWeapon()
{
	const int32 RandomSeed = FMath::Rand();
	FRandomStream WeaponRandomStream(RandomSeed);
	const float CurrentSpread = GetCurrentSpread();
	const float ConeHalfAngle = FMath::DegreesToRadians(CurrentSpread * 0.5f);

	const FVector AimDir = GetAdjustedAim();
	const FVector StartTrace = GetCameraDamageStartLocation(AimDir);
	const FVector ShootDir = WeaponRandomStream.VRandCone(AimDir, ConeHalfAngle, ConeHalfAngle);
	const FVector EndTrace = StartTrace + ShootDir * InstantConfig.WeaponRange;

	const FHitResult Impact = WeaponTrace(StartTrace, EndTrace);
	ProcessInstantHit(Impact, StartTrace, ShootDir, RandomSeed, CurrentSpread);

	CurrentFiringSpread = FMath::Min(InstantConfig.FiringSpreadMax, CurrentFiringSpread + InstantConfig.FiringSpreadIncrement);
}
