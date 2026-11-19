#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"
#include "OrbitCameraComponent.generated.h"

UCLASS(ClassGroup=(Camera), meta=(BlueprintSpawnableComponent))
class CSUNIVERSAL_API UOrbitCameraComponent : public UActorComponent
{
    GENERATED_BODY()

public:
    UOrbitCameraComponent();

    void UpdateRotationInput(
        FGlobalCameraState& TargetState,
        const FGlobalCameraState& CurrentState,
        AActor* ConnectTarget,
        float LookX,
        float LookY) const;

    float CalculateOrbitYaw(
        const FGlobalCameraState& State,
        AActor* PlayerTarget,
        AActor* ConnectTarget) const;

    bool CalculateDesiredLocation(
        const FGlobalCameraState& State,
        AActor* FollowTarget,
        AActor* PlayerTarget,
        AActor* ConnectTarget,
        FVector& OutDesiredLocation) const;
};