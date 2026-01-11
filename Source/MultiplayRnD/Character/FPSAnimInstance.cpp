
#include "FPSAnimInstance.h"

#include "FPSCharacter.h"
#include "Runtime/AnimGraphRuntime/Public/KismetAnimationLibrary.h"
#include "GameFramework/CharacterMovementComponent.h"

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
	bIsAiming = FPSCharacter->IsAiming();
}
