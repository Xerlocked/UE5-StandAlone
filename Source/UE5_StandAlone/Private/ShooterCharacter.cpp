// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterCharacter.h"

#include "BaseWeapon.h"
#include "SAGameMode.h"
#include "SAUsableActor.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Kismet/KismetMathLibrary.h"

// Sets default values
AShooterCharacter::AShooterCharacter()
{
	FPSCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FPSCamera"));
	FPSCamera->SetupAttachment(GetMesh());
	
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("PawnMesh1P"));
	Mesh1P->SetupAttachment(FPSCamera);
	
	Mesh1P->bOnlyOwnerSee = true;
	Mesh1P->bOwnerNoSee = false;
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->bReceivesDecals = false;
	
	Mesh1P->VisibilityBasedAnimTickOption = EVisibilityBasedAnimTickOption::OnlyTickPoseWhenRendered;
	Mesh1P->PrimaryComponentTick.TickGroup = TG_PrePhysics;
	
	Mesh1P->SetCollisionObjectType(ECC_Pawn);
	Mesh1P->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	Mesh1P->SetCollisionResponseToAllChannels(ECR_Ignore);

	GetMesh()->bOnlyOwnerSee = false;
	GetMesh()->bOwnerNoSee = true;
	GetMesh()->bReceivesDecals = false;
	
	GetMesh()->SetCollisionObjectType(ECC_Pawn);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
	GetMesh()->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	GetCapsuleComponent()->SetCollisionResponseToChannel(ECC_Camera, ECR_Ignore);
	
	BaseTurnRate = 45.f;
	BaseLookUpRate = 45.f;

	SprintSpeedModifier = 2.0f;
	ZoomSpeedModifier = 0.4f;
	Health = 100.f;
	TimeElapsed = 0.f;
	
	WeaponAttachPoint = TEXT("WeaponSocket");
}

// Called when the game starts or when spawned
void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();
	
}

float AShooterCharacter::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator,
	AActor* DamageCauser)
{
	if(Health <= 0.f)
	{
		return 0.f;
	}

	/* Modify based based on gametype rules */
	ASAGameMode* MyGameMode = Cast<ASAGameMode>(GetWorld()->GetAuthGameMode());
	Damage = MyGameMode ? MyGameMode->ModifyDamage(Damage, this, DamageEvent, EventInstigator, DamageCauser) : Damage;
	
	const float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	
	if (ActualDamage > 0.f)
	{
		Health -= ActualDamage;
		GEngine->AddOnScreenDebugMessage(-1, 1, FColor::Red, FString::Printf(TEXT("%f"), Health));
	}
	
	// 	if (Health <= 0)
	// 	{
	// 		bool bCanDie = true;
	//
	// 		/* Check the damagetype, always allow dying if the cast fails, otherwise check the property if player can die from damagetype */
	// 		if (DamageEvent.DamageTypeClass)
	// 		{
	// 			USDamageType* DmgType = Cast<USDamageType>(DamageEvent.DamageTypeClass->GetDefaultObject());
	// 			bCanDie = (DmgType == nullptr || (DmgType && DmgType->GetCanDieFrom()));
	// 		}
	//
	// 		if (bCanDie)
	// 		{
	// 			Die(ActualDamage, DamageEvent, EventInstigator, DamageCauser);
	// 		}
	// 		else
	// 		{
	// 			/* Player cannot die from this damage type, set hitpoints to 1.0 */
	// 			Health = 1.0f;
	// 		}
	// 	}
	// 	else
	// 	{
	// 		/* Shorthand for - if x != null pick1 else pick2 */
	// 		APawn* Pawn = EventInstigator ? EventInstigator->GetPawn() : nullptr;
	// 		PlayHit(ActualDamage, DamageEvent, Pawn, DamageCauser, false);
	// 	}
	// }

	return ActualDamage;
	
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
}

// Called every frame
void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (bWantsToRun && !IsSprinting())
	{
		SetSprinting(true);
	}
	
	SetArmTransform(DeltaTime);
}

#pragma region Movement

void AShooterCharacter::MoveForward(float Value)
{
	if (Controller && Value != 0.f)
	{
		// Limit pitch when walking or falling
		const bool bLimitRotation = (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling());
		const FRotator Rotation = bLimitRotation ? GetActorRotation() : Controller->GetControlRotation();
		const FVector Direction = FRotationMatrix(Rotation).GetScaledAxis(EAxis::X);
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveRight(float Value)
{
	if (Value != 0.f)
	{
		const FQuat Rotation = GetActorQuat();
		const FVector Direction = FQuatRotationMatrix(Rotation).GetScaledAxis(EAxis::Y);
		AddMovementInput(Direction, Value);
	}
}

void AShooterCharacter::MoveUp(float Value)
{
	if (Value != 0.f)
	{
		// Not when walking or falling.
		if (GetCharacterMovement()->IsMovingOnGround() || GetCharacterMovement()->IsFalling())
		{
			return;
		}

		AddMovementInput(FVector::UpVector, Value);
	}
}

void AShooterCharacter::TurnAtRate(float Value)
{
	// calculate delta for this frame from the rate information
	AddControllerYawInput(Value * BaseTurnRate * GetWorld()->GetDeltaSeconds());
}

void AShooterCharacter::LookUpAtRate(float Value)
{
	// calculate delta for this frame from the rate information
	AddControllerPitchInput(Value * BaseLookUpRate * GetWorld()->GetDeltaSeconds());
}
#pragma endregion Movement

void AShooterCharacter::Use()
{
	ASAUsableActor* Usable = GetUsableInView();
	if (Usable && !CurrentWeapon)
	{
		Usable->OnUsed(this);
	}
}


void AShooterCharacter::OnJump()
{
	SetIsJumping(true);
}


void AShooterCharacter::OnStopRunning()
{
	SetSprinting(false);
}


void AShooterCharacter::OnStartRunning()
{
	SetSprinting(true);
}


void AShooterCharacter::OnStartZoomIn()
{
	SetZoomIn(true);

	FPSCamera->FieldOfView = 55.f;
}


void AShooterCharacter::OnStopZoomIn()
{
	SetZoomIn(false);

	FPSCamera->FieldOfView = 90.f;
}


void AShooterCharacter::OnFireStart()
{
	if (IsSprinting())
	{
		SetSprinting(false);
	}
	
	StartWeaponFire();
}


void AShooterCharacter::OnFireStop()
{
	StopWeaponFire();
}


void AShooterCharacter::StartWeaponFire()
{
	if (!bWantsToFire)
	{
		bWantsToFire = true;
		if (CurrentWeapon)
		{
			CurrentWeapon->StartFire();
		}
	}
}


void AShooterCharacter::StopWeaponFire()
{
	if (bWantsToFire)
	{
		bWantsToFire = false;
		if (CurrentWeapon)
		{
			CurrentWeapon->StopFire();
		}
	}
}


void AShooterCharacter::SetIsJumping(bool NewState)
{
	if (NewState != bIsJumping)
	{
		bIsJumping = NewState;

		if(bIsJumping)
		{
			Jump();
		}
	}
	
}


void AShooterCharacter::SetSprinting(bool NewState)
{
	bWantsToRun = NewState;
}


bool AShooterCharacter::IsZoomIn() const
{
	return bIsZoomIn;
}


bool AShooterCharacter::IsSprinting() const
{
	if(!GetCharacterMovement())
	{
		return false;
	}
	return bWantsToRun && !IsZoomIn() && !GetVelocity().IsZero()
	&& (FVector::DotProduct(GetVelocity().GetSafeNormal2D(), GetActorRotation().Vector()) > 0.8);
}


bool AShooterCharacter::IsFiring() const
{
	return CurrentWeapon && CurrentWeapon->GetCurrentState() == EWeaponState::Firing;
}


void AShooterCharacter::AddWeapon(ABaseWeapon* NewWeapon)
{
	check(NewWeapon);
	if (NewWeapon)
	{
		EquipWeapon(NewWeapon);
	}
}


void AShooterCharacter::EquipWeapon(ABaseWeapon* NewWeapon)
{
	if (NewWeapon)
	{
		if (NewWeapon == CurrentWeapon)
		{
			return;
		}

		SetCurrentWeapon(NewWeapon, CurrentWeapon);
	}
}


void AShooterCharacter::SetCurrentWeapon(ABaseWeapon* NewWeapon, ABaseWeapon* LastWeapon)
{
	CurrentWeapon = NewWeapon;

	if (NewWeapon)
	{
		NewWeapon->SetOwningPawn(this);
		/* Only play equip animation when we already hold an item in hands */
		NewWeapon->OnEquip(false);
	}
}


EWeaponName AShooterCharacter::GetCurrentWeaponName() const
{
	if(CurrentWeapon)
	{
		return CurrentWeapon->GetWeaponName();
	}

	return EWeaponName::Knife;
}


UCameraComponent* AShooterCharacter::GetPlayerFPSCamera() const
{
	return FPSCamera;
}


ASAUsableActor* AShooterCharacter::GetUsableInView() const
{
	FVector CamLoc;
	FRotator CamRot;

	if (Controller == nullptr)
		return nullptr;

	Controller->GetPlayerViewPoint(CamLoc, CamRot); // 플레이어 뷰 포인트
	const FVector TraceStart = CamLoc; 
	const FVector Direction = CamRot.Vector();
	const FVector TraceEnd = TraceStart + (Direction * 1000);

	FCollisionQueryParams TraceParams(TEXT("TraceUsableActor"), true, this);
	TraceParams.bReturnPhysicalMaterial = false;

	/* Not tracing complex uses the rough collision instead making tiny objects easier to select. */
	TraceParams.bTraceComplex = false;

	FHitResult Hit(ForceInit);
	GetWorld()->LineTraceSingleByChannel(Hit, TraceStart, TraceEnd, ECC_Visibility, TraceParams);

	return Cast<ASAUsableActor>(Hit.GetActor());
}


bool AShooterCharacter::IsFirstPerson() const
{
	return IsAlive() && Controller && Controller->IsLocalPlayerController();
}


FName AShooterCharacter::GetWeaponAttachPoint() const
{
	return WeaponAttachPoint;
}


USkeletalMeshComponent* AShooterCharacter::GetSpecificPawnMesh(bool WantFirstPerson) const
{
	return WantFirstPerson == true ? Mesh1P : GetMesh();
}


USkeletalMeshComponent* AShooterCharacter::GetPawnMesh() const
{
	return IsFirstPerson() ? Mesh1P : GetMesh();
}


FRotator AShooterCharacter::GetAimOffsets() const
{
	const FVector AimDirWS = GetBaseAimRotation().Vector();
	const FVector AimDirLS = ActorToWorld().InverseTransformVectorNoScale(AimDirWS);
	const FRotator AimRotLS = AimDirLS.Rotation();

	return AimRotLS;
}


float AShooterCharacter::GetSprintSpeedModifier() const
{
	return SprintSpeedModifier;
}


float AShooterCharacter::GetZoomSpeedModifier() const
{
	return ZoomSpeedModifier;
}


void AShooterCharacter::OnCameraUpdate(const FVector& CameraLocation, const FRotator& CameraRotation) const
{
	USkeletalMeshComponent* DefMesh1P = Cast<USkeletalMeshComponent>(GetClass()->GetDefaultSubobjectByName(TEXT("PawnMesh1P")));
	const FMatrix DefMeshLS = FRotationTranslationMatrix(DefMesh1P->GetRelativeRotation(), DefMesh1P->GetRelativeLocation());
	const FMatrix LocalToWorld = ActorToWorld().ToMatrixWithScale();

	// Mesh rotating code expect uniform scale in LocalToWorld matrix

	const FRotator RotCameraPitch(CameraRotation.Pitch, 0.0f, 0.0f);
	const FRotator RotCameraYaw(0.0f, CameraRotation.Yaw, 0.0f);

	const FMatrix LeveledCameraLS = FRotationTranslationMatrix(RotCameraYaw, CameraLocation) * LocalToWorld.Inverse();
	const FMatrix PitchedCameraLS = FRotationMatrix(RotCameraPitch) * LeveledCameraLS;
	const FMatrix MeshRelativeToCamera = DefMeshLS * LeveledCameraLS.Inverse();
	const FMatrix PitchedMesh = MeshRelativeToCamera * PitchedCameraLS;

	Mesh1P->SetRelativeLocationAndRotation(PitchedMesh.GetOrigin(), PitchedMesh.Rotator());
}


void AShooterCharacter::OnMovementModeChanged(EMovementMode PrevMovementMode, uint8 PreviousCustomMode)
{
	Super::OnMovementModeChanged(PrevMovementMode, PreviousCustomMode);

	if (PrevMovementMode == EMovementMode::MOVE_Falling && 
	GetCharacterMovement()->MovementMode != EMovementMode::MOVE_Falling)
	{
		SetIsJumping(false);
	}
}


void AShooterCharacter::SetZoomIn(bool bNewState)
{
	bIsZoomIn = bNewState;
}


void AShooterCharacter::SetArmTransform(float DeltaTime)
{
	TimeElapsed = FMath::Clamp(TimeElapsed, 0, 1);

	const FTransform StartTransform = bIsZoomIn ? DefaultArmTransform : AimArmTransform;
	const FTransform EndTransform = bIsZoomIn ? AimArmTransform : DefaultArmTransform;
	
	Mesh1P->SetRelativeTransform(UKismetMathLibrary::TLerp(StartTransform, EndTransform, TimeElapsed));

	if(bIsZoomIn)
	{
		TimeElapsed += DeltaTime * 10;
	}
	else
	{
		TimeElapsed -= DeltaTime * 10;
	}
}


bool AShooterCharacter::IsAlive() const
{
	return Health > 0;
}


bool AShooterCharacter::CanFire() const
{
	return IsAlive();
}


void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
}

// Called to bind functionality to input
void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	PlayerInputComponent->BindAxis("MoveForward", this, &AShooterCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AShooterCharacter::MoveRight);
	PlayerInputComponent->BindAxis("MoveUp", this, &AShooterCharacter::MoveUp);
	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("TurnRate", this, &AShooterCharacter::TurnAtRate);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);

	PlayerInputComponent->BindAction("Use", IE_Pressed, this, &AShooterCharacter::Use);
	
	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &AShooterCharacter::OnJump);
	
	PlayerInputComponent->BindAction("Run", IE_Pressed, this, &AShooterCharacter::OnStartRunning);
	PlayerInputComponent->BindAction("Run", IE_Released, this, &AShooterCharacter::OnStopRunning);

	PlayerInputComponent->BindAction("Zoom", IE_Pressed, this, &AShooterCharacter::OnStartZoomIn);
	PlayerInputComponent->BindAction("Zoom", IE_Released, this, &AShooterCharacter::OnStopZoomIn);

	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AShooterCharacter::OnFireStart);
	PlayerInputComponent->BindAction("Fire", IE_Released, this, &AShooterCharacter::OnFireStop);
}


float AShooterCharacter::PlayAnimMontage(UAnimMontage* AnimMontage, float InPlayRate, FName StartSectionName)
{
	UAnimInstance * AnimInstance = (GetMesh1P())? GetMesh1P()->GetAnimInstance() : nullptr; 
	if( AnimMontage && AnimInstance )
	{
		float const Duration = AnimInstance->Montage_Play(AnimMontage, InPlayRate);

		if (Duration > 0.f)
		{
			// Start at a given Section.
			if( StartSectionName != NAME_None )
			{
				AnimInstance->Montage_JumpToSection(StartSectionName, AnimMontage);
			}

			return Duration;
		}
	}	
	return 0.f;
}

