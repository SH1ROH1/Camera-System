#pragma once


#include "CoreMinimal.h"
#include "Camera/Eventer/CameraPathSettings.h"
#include "Camera/GlobalCameraState.h"

#include "CameraEventerActionSettings.generated.h"

USTRUCT(BlueprintType)
struct FCameraEventerActionSettings {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  FGlobalCameraState CameraState;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  EGlobalCameraTransitionMode TransitionMode =
      EGlobalCameraTransitionMode::Smooth;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  TObjectPtr<AActor> ConnectTarget = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  TObjectPtr<AActor> CameraPathActor = nullptr;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  FCameraPathSettings PathSettings;
};