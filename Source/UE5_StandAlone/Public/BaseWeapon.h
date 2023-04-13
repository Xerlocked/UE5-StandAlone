// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseWeapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponName : uint8
{
	Knife = 0 UMETA(DisplayName = "Knife"),
	Ak47 UMETA(DisplayName = "Ak47"),
	Eagle UMETA(DisplayName = "Eagle")
};


UENUM()
enum class EWeaponType : uint8
{
	Pistol = 1 UMETA(DisplayName = "Pistol"),
	AssaultRifle UMETA(DisplayName = "Assault"),
	Sniper UMETA(DisplayName = "Sniper")
};

UENUM()
enum class EWeaponState : uint8
{
	Idle,
	Firing,
	Equipping,
	Reloading
};

USTRUCT()
struct FWeaponData
{
	GENERATED_BODY()

	/** infinite ammo for reloads */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteAmmo;

	/** infinite ammo in clip, no reload required */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	bool bInfiniteClip;

	/** max ammo */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 MaxAmmo;

	/** clip size */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 AmmoPerClip;

	/** initial clips */
	UPROPERTY(EditDefaultsOnly, Category=Ammo)
	int32 InitialClips;

	FWeaponData()
	{
		bInfiniteClip = false;
		bInfiniteAmmo = false;
		MaxAmmo = 300;
		AmmoPerClip = 30;
		InitialClips = 30;
	}
};


UCLASS(Abstract, Blueprintable)
class UE5_STANDALONE_API ABaseWeapon : public AActor
{
	GENERATED_BODY()

	FTimerHandle TimerHandle_HandleFiring;

	virtual void PostInitializeComponents() override;
	
public:	
	ABaseWeapon();

	virtual void OnEquip(bool bPlayAnimation);

	virtual void OnEquipFinished();

	bool IsEquipped() const;

	bool IsAttachedToPawn() const;
	
	void StartFire();

	void StopFire();

	EWeaponState GetCurrentState() const;

	EWeaponName GetWeaponName() const;

	EWeaponType GetWeaponType() const;
	
	/** attaches weapon mesh to pawn's mesh */
	void AttachMeshToPawn();

	/** detaches weapon mesh from pawn */
	void DetachMeshFromPawn();

	void SetOwningPawn(class AShooterCharacter* NewOwner);
	
protected:
	UPROPERTY()
	TObjectPtr<AShooterCharacter> MyPawn;

	/** weapon data */
	UPROPERTY(EditDefaultsOnly, Category=Config)
	FWeaponData WeaponConfig;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	FName AttachSocketName;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	EWeaponType WeaponType;

	UPROPERTY(EditDefaultsOnly, Category=Weapon)
	EWeaponName WeaponName;

	UPROPERTY(EditDefaultsOnly, Category=Effects)
	FName MuzzleAttachPoint;

	uint32 bIsEquipped : 1;

	uint32 bPendingEquip : 1;
	
	virtual FVector GetAdjustedAim() const;

	FVector GetCameraAim() const;

	FVector GetMuzzleLocation() const;

	FVector GetMuzzleDirection() const;

	FVector GetCameraDamageStartLocation(const FVector& AimDir) const;

	FHitResult WeaponTrace(const FVector& From, const FVector& To) const;

	virtual void FireWeapon() PURE_VIRTUAL(ABaseWeapon::FireWeapon,);

	USkeletalMeshComponent* GetWeaponMesh() const;

	TObjectPtr<class AShooterCharacter> GetPawnOwner() const;

	bool CanFire() const;

	virtual void SimulateWeaponFire();

	virtual void StopSimulatingWeaponFire();

	float PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate = 1.f, FName StartSectionName = NAME_None);

	void StopWeaponAnimation(UAnimMontage* Animation);
private:
	/** weapon mesh: 1st person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh1P;

	/** weapon mesh: 3rd person view */
	UPROPERTY(VisibleDefaultsOnly, Category=Mesh)
	USkeletalMeshComponent* Mesh3P;

	UPROPERTY(EditDefaultsOnly)
	UParticleSystem* MuzzleFX;

	UPROPERTY(Transient)
	UParticleSystemComponent* MuzzlePSC;

	UPROPERTY(EditDefaultsOnly)
	UAnimMontage* FireAnim;

	UPROPERTY(EditDefaultsOnly)
	FName MuzzleSocketName;

	bool bPlayingFireAnim;
	
	EWeaponState CurrentWeaponState;

	bool bWantsToFire;

	bool bReFiring;

	float LastFireTime;

	float TimeBetweenShots;

	virtual void HandleFiring();
	
	void SetWeaponState(EWeaponState NewState);

	void DetermineWeaponState();

	void OnBurstStarted();

	void OnBurstFinished();
};
