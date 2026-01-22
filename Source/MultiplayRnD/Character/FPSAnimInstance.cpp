
#include "FPSAnimInstance.h"

#include "FPSCharacter.h"
#include "Runtime/AnimGraphRuntime/Public/KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MultiplayRnD/Weapon/Weapon.h"

void UFPSAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();

	FPSCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
}

void UFPSAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);

	if (FPSCharacter == nullptr)
	{
		FPSCharacter = Cast<AFPSCharacter>(TryGetPawnOwner());
	}
	if (FPSCharacter == nullptr) return;

	// Set Character State
	FVector Velocity = FPSCharacter->GetVelocity();
	Velocity.Z = 0.f;
	Speed = Velocity.Size();

	FRotator Rotation = FPSCharacter->GetActorRotation();
	Direction = UKismetAnimationLibrary::CalculateDirection(Velocity, Rotation);

	AO_Pitch = FPSCharacter->GetAO_Pitch();
	
	bInAir = FPSCharacter->GetCharacterMovement()->IsFalling();
	bIsAccelerating = FPSCharacter->GetCharacterMovement()->GetCurrentAcceleration().Size() > 0.f;
	bIsCrouch = FPSCharacter->GetCharacterMovement()->IsCrouching();
	bWeaponEquiped = FPSCharacter->IsWeaponEquiped();
	EquippedWeapon = FPSCharacter->GetEquipWeapon();
	bIsAiming = FPSCharacter->IsAiming();

	// Weapon Left Hand IK
	if (bWeaponEquiped && EquippedWeapon && EquippedWeapon->GetWeaponMesh() && FPSCharacter->GetMesh())
	{
		LeftHandTransform = EquippedWeapon->GetWeaponMesh()->GetSocketTransform(FName("LeftHandSocket"), RTS_World);

		FVector OutPosition;
		FRotator OutRotation;
		FPSCharacter->GetMesh()->TransformToBoneSpace(FName("hand_r"), LeftHandTransform.GetLocation(), FRotator::ZeroRotator, OutPosition, OutRotation);
		LeftHandTransform.SetLocation(OutPosition);
		LeftHandTransform.SetRotation(FQuat(OutRotation));
	}
}
