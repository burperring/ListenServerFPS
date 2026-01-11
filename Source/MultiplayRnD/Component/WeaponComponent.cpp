
#include "WeaponComponent.h"

#include "Components/SphereComponent.h"
#include "Engine/SkeletalMeshSocket.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MultiplayRnD/Character/FPSCharacter.h"
#include "MultiplayRnD/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"


UWeaponComponent::UWeaponComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);

	fBaseWalkSpeed = 600.0f;
	fAimWalkSpeed = 450.0f;
}

void UWeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = fBaseWalkSpeed;
	}
}

void UWeaponComponent::SetAiming(bool _IsAiming)
{
	bAiming = _IsAiming;
	ServerSetAiming(_IsAiming);

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? fAimWalkSpeed : fBaseWalkSpeed;
	}
}

void UWeaponComponent::ServerSetAiming_Implementation(bool _IsAiming)
{
	bAiming = _IsAiming;

	if (Character)
	{
		Character->GetCharacterMovement()->MaxWalkSpeed = bAiming ? fAimWalkSpeed : fBaseWalkSpeed;
	}
}

void UWeaponComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

void UWeaponComponent::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UWeaponComponent, EquippedWeapon);
	DOREPLIFETIME(UWeaponComponent, bAiming);
}

void UWeaponComponent::EquipWeapon(AWeapon* _Weapon)
{
	if (Character == nullptr || _Weapon == nullptr) return;

	EquippedWeapon = _Weapon;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	
	const USkeletalMeshSocket* HandSocket = Character->GetMesh()->GetSocketByName(FName("Weapon_Socket"));
	if (HandSocket)
	{
		HandSocket->AttachActor(EquippedWeapon, Character->GetMesh());
	}

	EquippedWeapon->SetOwner(Character);
	EquippedWeapon->ShowPickUpWidget(false);
	EquippedWeapon->GetAreaSphere()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}






