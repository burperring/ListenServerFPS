// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Teleporter/VREditorTeleporter.h"
#include "WeaponComponent.generated.h"

class AWeapon;
class AFPSCharacter;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MULTIPLAYRND_API UWeaponComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UWeaponComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<class FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipWeapon(AWeapon* _Weapon);
	
	void SetAiming(bool _IsAiming);
	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool _IsAiming);

	FORCEINLINE void SetCharacter(AFPSCharacter* _Character) { Character = _Character; };
	FORCEINLINE AWeapon* GetEquippedWeapon() const { return EquippedWeapon; }
	FORCEINLINE bool GetAiming() const { return bAiming; }

protected:
	virtual void BeginPlay() override;

private:
	UPROPERTY()
	AFPSCharacter* Character;
	UPROPERTY(Replicated)
	AWeapon* EquippedWeapon;
	UPROPERTY(Replicated)
	bool bAiming;

	UPROPERTY(EditAnywhere)
	float fBaseWalkSpeed;
	UPROPERTY(EditAnywhere)
	float fAimWalkSpeed;
};
