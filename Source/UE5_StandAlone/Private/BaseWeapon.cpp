// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseWeapon.h"

#include "ShooterCharacter.h"
#include "ShooterPlayerController.h"
#include "Kismet/KismetSystemLibrary.h"


void ABaseWeapon::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	TimeBetweenShots = 60.0f / 700;
}

ABaseWeapon::ABaseWeapon()
{
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh1P"));
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->bReceivesDecals = false;
	Mesh1P->CastShadow = false;
	Mesh1P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);
	RootComponent = Mesh1P;

	Mesh3P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh3P"));
	Mesh3P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh3P->bReceivesDecals = false;
	Mesh3P->CastShadow = true;
	Mesh3P->SetCollisionObjectType(ECC_WorldDynamic);
	Mesh3P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh3P->SetCollisionResponseToAllChannels(ECR_Ignore);
	//Mesh3P->SetCollisionResponseToChannel(Weapon, ECR_Block);
	Mesh3P->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);
	// Mesh3P->SetCollisionResponseToChannel(COLLISION_PROJECTILE, ECR_Block);
	Mesh3P->SetupAttachment(Mesh1P);

	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.TickGroup = TG_PrePhysics;
	
	CurrentWeaponState = EWeaponState::Idle;
	WeaponConfig.bInfiniteAmmo = false;
	WeaponConfig.bInfiniteClip = false;
	WeaponConfig.MaxAmmo = 300;
	WeaponConfig.AmmoPerClip = 30;
	WeaponConfig.InitialClips = 30;
}

void ABaseWeapon::OnEquip(bool bPlayAnimation)
{
	bPendingEquip = true;
	DetermineWeaponState();
	
	OnEquipFinished();
}

void ABaseWeapon::OnEquipFinished()
{
	AttachMeshToPawn();

	bIsEquipped = true;
	bPendingEquip = false;
}

bool ABaseWeapon::IsEquipped() const
{
	return bIsEquipped;
}

bool ABaseWeapon::IsAttachedToPawn() const
{
	return bIsEquipped || bPendingEquip;
}

void ABaseWeapon::StartFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		DetermineWeaponState();
	}
}

void ABaseWeapon::StopFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		DetermineWeaponState();
	}
}

EWeaponState ABaseWeapon::GetCurrentState() const
{
	return CurrentWeaponState;
}

EWeaponName ABaseWeapon::GetWeaponName() const
{
	return WeaponName;
}

EWeaponType ABaseWeapon::GetWeaponType() const
{
	return WeaponType;
}

USkeletalMeshComponent* ABaseWeapon::GetMesh1P() const
{
	return Mesh1P;
}

FVector ABaseWeapon::GetAdjustedAim() const
{
	const AShooterPlayerController* PlayerController = GetInstigatorController<AShooterPlayerController>();

	FVector FinalAim = FVector::ZeroVector;
	// If we have a player controller use it for the aim
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}

	return FinalAim;
}

FVector ABaseWeapon::GetCameraAim() const
{
	const AShooterPlayerController* PlayerController = GetInstigatorController<AShooterPlayerController>();

	FVector FinalAim = FVector::ZeroVector;
	
	if (PlayerController)
	{
		FVector CamLoc;
		FRotator CamRot;
		PlayerController->GetPlayerViewPoint(CamLoc, CamRot);
		FinalAim = CamRot.Vector();
	}
	else if (GetInstigator())
	{
		FinalAim = GetInstigator()->GetBaseAimRotation().Vector();		
	}
	
	return FinalAim;
}

FVector ABaseWeapon::GetMuzzleLocation() const
{
	const USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketLocation(MuzzleAttachPoint);
}

FVector ABaseWeapon::GetMuzzleDirection() const
{
	const USkeletalMeshComponent* UseMesh = GetWeaponMesh();
	return UseMesh->GetSocketRotation(MuzzleAttachPoint).Vector();
}

FVector ABaseWeapon::GetCameraDamageStartLocation(const FVector& AimDir) const
{
	AShooterPlayerController* PC = MyPawn ? Cast<AShooterPlayerController>(MyPawn.Get()->Controller) : nullptr;
	FVector OutStartTrace = FVector::ZeroVector;
	
	if (PC)
	{
		// use player's camera
		FRotator UnusedRot;
		PC->GetPlayerViewPoint(OutStartTrace, UnusedRot);
		// Adjust trace so there is nothing blocking the ray between the camera and the pawn, and calculate distance from adjusted start
		OutStartTrace = OutStartTrace + AimDir * ((GetInstigator()->GetActorLocation() - OutStartTrace) | AimDir);
	}
	return OutStartTrace;
}

FHitResult ABaseWeapon::WeaponTrace(const FVector& From, const FVector& To) const
{
	// Perform trace to retrieve hit info
	FCollisionQueryParams TraceParams(SCENE_QUERY_STAT(WeaponTrace), true, GetInstigator());
	TraceParams.bReturnPhysicalMaterial = true;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, From, To, ECC_Visibility, TraceParams);

	if(Hit.GetActor())
	{
		UE_LOG(LogTemp, Log, TEXT("%s"), *Hit.GetActor()->GetName());
	}

	FHitResult HitResultDebug = Hit;

	TArray<AActor*> ActorsToIgnore;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), From, To,
		ETraceTypeQuery::TraceTypeQuery1, false, ActorsToIgnore, EDrawDebugTrace::ForDuration, HitResultDebug, true);

	
	// Draw Debug Line
	// DrawDebugLine(
	// 			GetWorld(),
	// 			From,
	// 			To,
	// 			FColor(255, 0, 0),
	// 			false, 10, 0,
	// 			12.333
	// 		);
	return Hit;
}

USkeletalMeshComponent* ABaseWeapon::GetWeaponMesh() const
{
	return (MyPawn != nullptr) ? Mesh1P : Mesh3P;
}

TObjectPtr<class AShooterCharacter> ABaseWeapon::GetPawnOwner() const
{
	return MyPawn;
}

bool ABaseWeapon::CanFire() const
{
	const bool bPawnCanFire = MyPawn && MyPawn->CanFire();
	const bool bStateOK = CurrentWeaponState == EWeaponState::Idle || CurrentWeaponState == EWeaponState::Firing;
	return bPawnCanFire && bStateOK;
}

void ABaseWeapon::SimulateWeaponFire()
{
	if (MuzzleFX)
	{
		MuzzleNC = UNiagaraFunctionLibrary::SpawnSystemAttached(MuzzleFX, Mesh1P, MuzzleAttachPoint, FVector(1), FRotator(1), FVector(1), EAttachLocation::Type::SnapToTarget, true, ENCPoolMethod::AutoRelease);
	}

	if (!bPlayingFireAnim)
	{
		if(MyPawn->IsZoomIn())
		{
			PlayWeaponAnimation(FireAnimAds);
			SetCurrentFireAnim(FireAnimAds);
		}
		else
		{
			PlayWeaponAnimation(FireAnim);
			SetCurrentFireAnim(FireAnim);
		}
		bPlayingFireAnim = true;
	}
}

void ABaseWeapon::StopSimulatingWeaponFire()
{
	if (bPlayingFireAnim)
	{
		StopWeaponAnimation(CurrentFireAnim);
		bPlayingFireAnim = false;
	}
}

float ABaseWeapon::PlayWeaponAnimation(UAnimMontage* Animation, float InPlayRate, FName StartSectionName)
{
	float Duration = 0.0f;
	if (MyPawn)
	{
		if (Animation)
		{
			Duration = MyPawn->PlayAnimMontage(Animation, InPlayRate, StartSectionName);
		}
	}

	return Duration;
}

void ABaseWeapon::StopWeaponAnimation(UAnimMontage* Animation)
{
	if (MyPawn)
	{
		if (Animation)
		{
			MyPawn->StopAnimMontage(Animation);
		}
	}
}

void ABaseWeapon::HandleFiring()
{
	if ((WeaponConfig.AmmoPerClip > 0 || WeaponConfig.bInfiniteClip) && CanFire())
	{
		if (GetNetMode() != NM_DedicatedServer)
		{
			SimulateWeaponFire();
		}

		if (MyPawn && MyPawn->IsLocallyControlled())
		{
			FireWeapon();
		}
	}
	
	if (MyPawn && MyPawn->IsLocallyControlled())
	{
		/* Retrigger HandleFiring on a delay for automatic weapons */
		bReFiring = (CurrentWeaponState == EWeaponState::Firing && TimeBetweenShots > 0.0f);
		if (bReFiring)
		{
			GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, TimeBetweenShots, false);
		}
	}

	LastFireTime = GetWorld()->GetTimeSeconds();
}


void ABaseWeapon::DetermineWeaponState()
{
	EWeaponState NewState = EWeaponState::Idle;

	if (bIsEquipped)
	{
		if (bWantsToFire)
		{
			NewState = EWeaponState::Firing;
		}
	}
	else if (bPendingEquip)
	{
		NewState = EWeaponState::Equipping;
	}

	
	SetWeaponState(NewState);
}


void ABaseWeapon::SetWeaponState(EWeaponState NewState)
{
	const EWeaponState PrevState = CurrentWeaponState;

	if (PrevState == EWeaponState::Firing && NewState != EWeaponState::Firing)
	{
		OnBurstFinished();
	}

	CurrentWeaponState = NewState;

	if (PrevState != EWeaponState::Firing && NewState == EWeaponState::Firing)
	{
		OnBurstStarted();
	}
}


void ABaseWeapon::OnBurstStarted()
{
	// Start firing, can be delayed to satisfy TimeBetweenShots
	const float GameTime = GetWorld()->GetTimeSeconds();
	if (LastFireTime > 0 && TimeBetweenShots > 0.0f &&
		LastFireTime + TimeBetweenShots > GameTime)
	{
		GetWorldTimerManager().SetTimer(TimerHandle_HandleFiring, this, &ABaseWeapon::HandleFiring, LastFireTime + TimeBetweenShots - GameTime, false);
	}
	else
	{
		HandleFiring();
	}
}

void ABaseWeapon::OnBurstFinished()
{
	if (GetNetMode() != NM_DedicatedServer)
	{
		StopSimulatingWeaponFire();
	}

	GetWorldTimerManager().ClearTimer(TimerHandle_HandleFiring);
	bReFiring = false;
}

void ABaseWeapon::AttachMeshToPawn()
{
	if (MyPawn)
	{
		// Remove and hide both first and third person meshes
		DetachMeshFromPawn();

		// For locally controller players we attach both weapons and let the bOnlyOwnerSee, bOwnerNoSee flags deal with visibility.
		const FName AttachPoint = MyPawn->GetWeaponAttachPoint();
		if( MyPawn->IsLocallyControlled() == true )
		{
			USkeletalMeshComponent* PawnMesh1p = MyPawn->GetSpecificPawnMesh(true);
			USkeletalMeshComponent* PawnMesh3p = MyPawn->GetSpecificPawnMesh(false);
			Mesh1P->SetHiddenInGame( false );
			Mesh3P->SetHiddenInGame( false );
			Mesh1P->AttachToComponent(PawnMesh1p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			Mesh3P->AttachToComponent(PawnMesh3p, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
		}
		else
		{
			USkeletalMeshComponent* UseWeaponMesh = GetWeaponMesh();
			USkeletalMeshComponent* UsePawnMesh = MyPawn->GetPawnMesh();
			UseWeaponMesh->AttachToComponent(UsePawnMesh, FAttachmentTransformRules::KeepRelativeTransform, AttachPoint);
			UseWeaponMesh->SetHiddenInGame( false );
		}
	}
}

void ABaseWeapon::DetachMeshFromPawn()
{
	Mesh1P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh1P->SetHiddenInGame(true);

	Mesh3P->DetachFromComponent(FDetachmentTransformRules::KeepRelativeTransform);
	Mesh3P->SetHiddenInGame(true);
}

void ABaseWeapon::SetOwningPawn(AShooterCharacter* NewOwner)
{
	if (MyPawn != NewOwner)
	{
		SetInstigator(NewOwner);
		MyPawn = NewOwner;
		// // Net owner for RPC calls.
		// SetOwner(NewOwner);
	}
}

void ABaseWeapon::SetCurrentFireAnim(UAnimMontage *NewAnimMontage)
{
	if(CurrentFireAnim != NewAnimMontage)
	{
		CurrentFireAnim = NewAnimMontage;
	}
}
