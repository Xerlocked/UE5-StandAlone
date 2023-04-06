// Fill out your copyright notice in the Description page of Project Settings.


#include "SAUsableActor.h"

// Sets default values
ASAUsableActor::ASAUsableActor()
{
	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	RootComponent = MeshComp;
}

void ASAUsableActor::OnBeginFocus()
{
}

void ASAUsableActor::OnEndFocus()
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(true);
}

void ASAUsableActor::OnUsed(APawn* InstigatorPawn)
{
	// Used by custom PostProcess to render outlines
	MeshComp->SetRenderCustomDepth(false);
}

