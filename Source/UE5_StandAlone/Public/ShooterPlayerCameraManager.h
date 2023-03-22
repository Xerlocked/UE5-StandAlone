// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "ShooterPlayerCameraManager.generated.h"

/**
 * 
 */
UCLASS()
class UE5_STANDALONE_API AShooterPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

	AShooterPlayerCameraManager();

	
public:
	float NormalFOV;

	float ZoomFOV;

	virtual void UpdateCamera(float DeltaTime) override;
};
