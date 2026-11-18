
#pragma once

#include "CoreMinimal.h"
#include "GlobalCameraState.h"

#include "GlobalCameraEventerSettings.generated.h"

USTRUCT(BlueprintType)
struct FGlobalCameraEventerSettings {
  GENERATED_BODY()

  FName Name;

  // Camera.

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  FGlobalCameraState CameraState;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  FName ConnectTargetName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
  EGlobalCameraTransitionMode TransitionMode =
      EGlobalCameraTransitionMode::Smooth;

  // Path.

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  FName CameraPathActorName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path",
            meta = (ClampMin = "0.0"))
  float StartDistanceThreshold = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  float PathOffset = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path",
            meta = (ClampMin = "0.1"))
  float FollowSmoothing = 5.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  bool bPathLoop = false;

  // Restore.

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore")
  bool bRestoreOnExit = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore",
            meta = (EditCondition = "bRestoreOnExit"))
  FGlobalCameraState RestoreCameraState;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore",
            meta = (EditCondition = "bRestoreOnExit"))
  FName RestorePresetName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore",
            meta = (EditCondition = "bRestoreOnExit"))
  FName RestoreConnectTargetName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore",
            meta = (EditCondition = "bRestoreOnExit"))
  EGlobalCameraTransitionMode RestoreTransitionMode =
      EGlobalCameraTransitionMode::Smooth;

  // Restore Path.

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore | Path",
            meta = (EditCondition = "bRestoreOnExit"))
  FName RestoreCameraPathActorName;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore | Path",
            meta = (EditCondition = "bRestoreOnExit", ClampMin = "0.0"))
  float RestoreStartDistanceThreshold = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore | Path",
            meta = (EditCondition = "bRestoreOnExit"))
  float RestorePathOffset = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore | Path",
            meta = (EditCondition = "bRestoreOnExit", ClampMin = "0.1"))
  float RestoreFollowSmoothing = 5.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore | Path",
            meta = (EditCondition = "bRestoreOnExit"))
  bool bRestorePathLoop = false;

  // Trigger.

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
  bool bFireOnlyOnce = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger",
            meta = (ClampMin = "0.0"))
  float ActivationDelay = 0.f;
};
