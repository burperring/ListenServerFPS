#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "FPSCharacter.generated.h"

class UInputMappingContext;
class UInputAction;
class USpringArmComponent;
class UCameraComponent;
class USkeletalMeshComponent;
class UWeaponComponent;
class UCombatComponent;
class UWidgetComponent;
class AWeapon;

UCLASS()
class MULTIPLAYRND_API AFPSCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPSCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	
	// Input Mapping
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputMappingContext> MappingContext;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> MovementAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> JumpAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> LookAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> FireAction;
	UPROPERTY(EditAnywhere, Category = "Input")
    TObjectPtr<UInputAction> AimAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> CrouchAction;
	UPROPERTY(EditAnywhere, Category = "Input")
	TObjectPtr<UInputAction> EquipAction;
	
	void SetOverlappingWeapon(AWeapon* _Weapon);
	bool IsWeaponEquiped();
	bool IsAiming();

	FORCEINLINE float GetAO_Pitch() const { return AO_Pitch; };
	
protected:
	virtual void BeginPlay() override;

	void Move(const FInputActionValue& Value);
	void Look(const FInputActionValue& Value);
	void StartFire(const FInputActionValue& Value);
	void StopFire(const FInputActionValue& Value);
	void StartAim(const FInputActionValue& Value);
	void StopAim(const FInputActionValue& Value);
	void CrouchStart(const FInputActionValue& Value);
	void CrouchEnd(const FInputActionValue& Value);
	void Equip(const FInputActionValue& Value);
	virtual void Jump() override;

	void AimOffset(float DeltaTime);

private:
	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);
	UFUNCTION(Server, Reliable)
	void ServerEquip();
	void ServerEquip_Implementation();
	
protected:
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "CombatComp")
	UCombatComponent* CombatComponent;
	UPROPERTY(VisibleAnywhere, BlueprintReadWrite, Category = "WeaponComp")
	UWeaponComponent* WeaponComponent;

private:
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	USpringArmComponent* CameraBoom;
	UPROPERTY(VisibleAnywhere, Category = "Camera")
	UCameraComponent* FollowCamera;
	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UWidgetComponent* OverHeadWidgetComponent;

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	AWeapon* OverlappingWeapon;

	float AO_Pitch;
};
