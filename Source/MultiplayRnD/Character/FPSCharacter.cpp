#include "FPSCharacter.h"

#include "Camera/CameraComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "EnhancedPlayerInput.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "MultiplayRnD/Component/CombatComponent.h"
#include "MultiplayRnD/Component/WeaponComponent.h"
#include "MultiplayRnD/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"

const FName CameraBoomCompName = "CameraBoom";
const FName CameraComponentName = "FollowCamera";
const FName PlayerSkeletalMeshName = "PlayerFPSMesh";
const FName OverHeadWidgetName = "OverHeadWidgetComponent";
const FName WeaponComponentName = "WeaponComponent";
const FName CombatComponentName = "CombatComponent";

AFPSCharacter::AFPSCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(CameraBoomCompName);
	CameraBoom->SetupAttachment(GetMesh(), TEXT("camera_socket"));
	CameraBoom->TargetArmLength = 0.0f;
	CameraBoom->bUsePawnControlRotation = true;

	FollowCamera = CreateDefaultSubobject<UCameraComponent>(CameraComponentName);
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	OverHeadWidgetComponent = CreateDefaultSubobject<UWidgetComponent>(OverHeadWidgetName);
	OverHeadWidgetComponent->SetupAttachment(RootComponent);

	CombatComponent = CreateDefaultSubobject<UCombatComponent>(CombatComponentName);
	WeaponComponent = CreateDefaultSubobject<UWeaponComponent>(WeaponComponentName);

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AFPSCharacter, OverlappingWeapon, COND_OwnerOnly);
}

void AFPSCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	AimOffset(DeltaTime);
}

void AFPSCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();

	if (WeaponComponent)
	{
		WeaponComponent->SetCharacter(this);
	}
}

void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (ULocalPlayer* LocalPlayer = PC->GetLocalPlayer())
		{
			if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(LocalPlayer))
			{
				if (MappingContext)
				{
					Subsystem->ClearAllMappings();
					Subsystem->AddMappingContext(MappingContext, 0);
				}
			}
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		if (MovementAction) EnhancedInputComponent->BindAction(MovementAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Move);
        if (JumpAction) EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Jump);
		if (LookAction) EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AFPSCharacter::Look);
		if (FireAction)
		{
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AFPSCharacter::StartFire);
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopFire);
		}
		if (AimAction)
		{
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &AFPSCharacter::StartAim);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &AFPSCharacter::StopAim);
		}
		if (CrouchAction)
		{
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Started, this, &AFPSCharacter::CrouchStart);
			EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AFPSCharacter::CrouchEnd);
		}
		if (EquipAction) EnhancedInputComponent->BindAction(EquipAction, ETriggerEvent::Started, this, &AFPSCharacter::Equip);
	}
}

void AFPSCharacter::Move(const FInputActionValue& Value)
{
	if (!Controller) return;

	const FVector2D MoveInput = Value.Get<FVector2D>();

	if (!FMath::IsNearlyZero(MoveInput.X))
	{
		AddMovementInput(GetActorForwardVector(), MoveInput.X);
	}
	if (!FMath::IsNearlyZero(MoveInput.Y))
	{
		AddMovementInput(GetActorRightVector(), MoveInput.Y);
	}
}

void AFPSCharacter::Look(const FInputActionValue& Value)
{
	FVector2D LookInput = Value.Get<FVector2D>();

	AddControllerYawInput(LookInput.X);
	AddControllerPitchInput(LookInput.Y);
}

void AFPSCharacter::StartFire(const FInputActionValue& Value)
{
	
}

void AFPSCharacter::StopFire(const FInputActionValue& Value)
{
	
}

void AFPSCharacter::StartAim(const FInputActionValue& Value)
{
	if (!WeaponComponent) return;

	WeaponComponent->SetAiming(true);
}

void AFPSCharacter::StopAim(const FInputActionValue& Value)
{
	if (!WeaponComponent) return;

	WeaponComponent->SetAiming(false);
}

void AFPSCharacter::CrouchStart(const FInputActionValue& Value)
{
	if (CanCrouch())
	{
		Crouch();
	}
}

void AFPSCharacter::CrouchEnd(const FInputActionValue& Value)
{
	UnCrouch();
}

void AFPSCharacter::Equip(const FInputActionValue& Value)
{
	if (!WeaponComponent) return;

	if (HasAuthority())
		WeaponComponent->EquipWeapon(OverlappingWeapon);
	else
		ServerEquip();
}

void AFPSCharacter::ServerEquip_Implementation()
{
	if (!WeaponComponent) return;

	WeaponComponent->EquipWeapon(OverlappingWeapon);
}

void AFPSCharacter::Jump()
{
	Super::Jump();
}

void AFPSCharacter::AimOffset(float DeltaTime)
{
	if (WeaponComponent && WeaponComponent->GetEquippedWeapon() == nullptr) return;

	AO_Pitch = GetBaseAimRotation().Pitch;
	if (AO_Pitch > 90.0f && !IsLocallyControlled())
	{
		FVector2D InRange(270.0f, 360.0f);
		FVector2D OutRange(-90.0f, 0.0f);
		AO_Pitch = FMath::GetMappedRangeValueClamped(InRange, OutRange, AO_Pitch);
	}
}

void AFPSCharacter::SetOverlappingWeapon(AWeapon* _Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(false);
	}
	
	OverlappingWeapon = _Weapon;
	if (IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickUpWidget(true);
		}
	}
}

bool AFPSCharacter::IsWeaponEquiped()
{
	return WeaponComponent && WeaponComponent->GetEquippedWeapon();
}

bool AFPSCharacter::IsAiming()
{
	return WeaponComponent && WeaponComponent->GetAiming();
}

void AFPSCharacter::OnRep_OverlappingWeapon(AWeapon* LastWeapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickUpWidget(true);
	}
	if (LastWeapon)
	{
		LastWeapon->ShowPickUpWidget(false);
	}
}
