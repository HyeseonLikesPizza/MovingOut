// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/CountProps.h"


ACountProps::ACountProps()
{
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> PhysMaterialRef(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Assets/Characters/Player/PM_Sticky.PM_Sticky'"));
	if (PhysMaterialRef.Succeeded())
	{
		Mesh->SetPhysMaterialOverride(PhysMaterialRef.Object);
	}
}

void ACountProps::ChangeMaterial(UMaterial* NewMaterial)
{
	if (Mesh && NewMaterial)
	{
		Mesh->SetMaterial(0, NewMaterial);
	}
}

void ACountProps::SetDefaultMaterial()
{
	Mesh->SetMaterial(0, DefaultMaterial);
}