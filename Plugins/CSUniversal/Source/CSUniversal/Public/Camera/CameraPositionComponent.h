#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"
#include "CameraPositionComponent.generated.h"

class AActor;

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSUNIVERSAL_API UCameraPositionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UCameraPositionComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Position")
	FVector FixedWorldPosition = FVector::ZeroVector;

	void InitializePosition(const FGlobalCameraState& State, AActor* PositionTarget);

	void SetFixedWorldPosition(const FVector& NewPosition);

	FVector GetFixedWorldPosition() const;

	FVector GetWorldPositionFromState(
		const FGlobalCameraState& State,
		AActor* PositionTarget
	) const;

	FVector GetRelativePositionFromWorld(
		const FVector& WorldPosition,
		AActor* PositionTarget
	) const;

	void ConvertPositionSpace(
		FGlobalCameraState& State,
		EGlobalCameraPositionMode NewPositionMode,
		AActor* PositionTarget
	);
	
};