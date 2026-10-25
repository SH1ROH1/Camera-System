#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "FPCharacter.generated.h"

class ASystemCamera;

UCLASS()
class CSFIRSTPERSON_API AFPCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AFPCharacter();
	// Virtual functions
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;
	// Camera
	UFUNCTION(BlueprintPure, Category = "Camera")
	ASystemCamera* GetCameraActor() const;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	bool bUseMouseLook = true;

protected:
	// Camera
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TSubclassOf<ASystemCamera> CameraClass;
	UPROPERTY(BlueprintReadOnly, Category = "Camera")
	TObjectPtr<ASystemCamera> CameraActor;
	// Movement
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Movement")
	float MoveSpeed = 600.f;
	// Movement input
	float MoveX = 0.f;
	float MoveZ = 0.f;
	void Move_X(float Value);
	void Move_Z(float Value);
	void ApplyMovement(float DeltaTime);
	// Mouse input
	float LookX = 0.f;
	float LookY = 0.f;
	void Look_X(float Value);
	void Look_Y(float Value);
	void UpdateCameraRotation(float deltaTime) const;

public:
	// Input
	virtual void SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) override;
};