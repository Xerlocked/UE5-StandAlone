// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterAnimInstance.h"

#include "KismetAnimationLibrary.h"
#include "ShooterCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

UShooterAnimInstance::UShooterAnimInstance() : Speed(0), bIsJumping(false), bIsAccelerating(false),
JumpTime(0), bIsAiming(false), bIsFiring(false), bIsSprinting(false)
{
	
}

void UShooterAnimInstance::UpdateAnimationProperties(float DeltaTime)
{
	if (ShooterCharacter == nullptr)
	{
		ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
	}

	if(ShooterCharacter)
	{
		FVector Velocity { ShooterCharacter->GetVelocity() };
		Velocity.Z = 0;
		Speed = Velocity.Size();

		const FRotator ShooterRotator { ShooterCharacter->GetActorRotation() };

		Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, ShooterRotator);

		bIsJumping = ShooterCharacter->GetCharacterMovement()->Velocity.Z > 1.0f ? true : false;

		bIsAccelerating = ShooterCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.0f ? true : false;

		bIsAiming = ShooterCharacter->IsZoomIn();

		bIsFiring = ShooterCharacter->IsFiring();

		bIsSprinting = ShooterCharacter->IsSprinting();

		CurrentWeaponName = ShooterCharacter->GetCurrentWeaponName();
	}
}

void UShooterAnimInstance::NativeInitializeAnimation()
{
	ShooterCharacter = Cast<AShooterCharacter>(TryGetPawnOwner());
}
