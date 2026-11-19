#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"
#include "FirstPersonCameraComponent.generated.h"

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class CSUNIVERSAL_API UFirstPersonCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFirstPersonCameraComponent();

	void UpdateRotationInput(FGlobalCameraState& TargetState, float LookX, float LookY) const;

	bool CalculateDesiredLocation(
		const FGlobalCameraState& State,
		AActor* FollowTarget,
		FVector& OutDesiredLocation) const;
};