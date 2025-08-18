// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/NonCountProps.h"

ANonCountProps::ANonCountProps()
{
	static ConstructorHelpers::FObjectFinder<UMaterial> MaterialRef(TEXT("/Script/Engine.Material'/Game/Assets/Furniture_Free/Materials/M_Material.M_Material'"));
	if (MaterialRef.Succeeded())
	{
		DefaultMaterial = MaterialRef.Object;
	}
}
