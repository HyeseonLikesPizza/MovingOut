// Fill out your copyright notice in the Description page of Project Settings.


#include "Props/CountProps.h"

#include "Character/PlayerMovingOutCharacter.h"
#include "Component/InteractiveComponent.h"
#include "Components/BoxComponent.h"


ACountProps::ACountProps()
{
	BoxCollision = CreateDefaultSubobject<UBoxComponent>(TEXT("BoxCollision"));
	BoxCollision->SetupAttachment(GetRootComponent());
	
	static ConstructorHelpers::FObjectFinder<UPhysicalMaterial> PhysMaterialRef(TEXT("/Script/PhysicsCore.PhysicalMaterial'/Game/Assets/Characters/Player/PM_Sticky.PM_Sticky'"));
	if (PhysMaterialRef.Succeeded())
	{
		Mesh->SetPhysMaterialOverride(PhysMaterialRef.Object);
	}

	// 충돌 없이 오버랩만
	BoxCollision->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	BoxCollision->SetCollisionObjectType(ECC_WorldDynamic);

	//기본값 : Ignore
	BoxCollision->SetCollisionResponseToAllChannels(ECR_Ignore);
	BoxCollision->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	
	BoxCollision->SetGenerateOverlapEvents(true);
}

void ACountProps::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	BoxCollision->OnComponentBeginOverlap.AddDynamic(this, &ACountProps::OnBeginOverlap);
	BoxCollision->OnComponentEndOverlap.AddDynamic(this, &ACountProps::OnEndOverlap);
}

void ACountProps::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// If player character is grabbing this prop, turn off stencil
	if (CharacterComponentRef)
	{
		
	}
}

void ACountProps::OnBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
                                 UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	APlayerMovingOutCharacter* Character = Cast<APlayerMovingOutCharacter>(OtherActor);
	if (Character)
	{
		Mesh->SetRenderCustomDepth(true);

		CharacterComponentRef = Cast<UInteractiveComponent>(Character->InteractiveComponent);
		IsValid(CharacterComponentRef);
	}
}

void ACountProps::OnEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	Mesh->SetRenderCustomDepth(false);
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
