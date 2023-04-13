// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterPlayerCameraManager.h"
#include "ShooterCharacter.h"

AShooterPlayerCameraManager::AShooterPlayerCameraManager()
{
	NormalFOV = 90.f;
	ZoomFOV = 55.f;
	ViewPitchMax = 90.f;
	ViewPitchMin = -90.f;
	bAlwaysApplyModifiers = true;
}

void AShooterPlayerCameraManager::UpdateCamera(float DeltaTime)
{
	AShooterCharacter* MyPawn = PCOwner ? Cast<AShooterCharacter>(PCOwner->GetPawn()) : nullptr;
	if (MyPawn)
	{
		const float FOV = MyPawn->IsZoomIn() ? ZoomFOV : NormalFOV;
		DefaultFOV = FMath::FInterpTo(DefaultFOV,FOV, DeltaTime, 20.f);
		GetCameraViewPoint(DefaultCameraLocation, DefaultCameraRotator);
	}

	Super::UpdateCamera(DeltaTime);

	// if (MyPawn)
	// {
	// 	MyPawn->OnCameraUpdate(GetCameraLocation(), GetCameraRotation());
	// }
}
