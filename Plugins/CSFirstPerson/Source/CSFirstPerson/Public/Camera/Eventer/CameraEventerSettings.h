#pragma once

#include "Camera/Eventer/CameraEventerActionSettings.h"

#include "CoreMinimal.h"

#include "CameraEventerSettings.generated.h"
USTRUCT(BlueprintType)
struct FCameraEventerSettings {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Activation")
  FCameraEventerActionSettings Activation;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore")
  bool bRestoreOnExit = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Restore",
            meta = (EditCondition = "bRestoreOnExit", EditConditionHides))
  FCameraEventerActionSettings Restore;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
  bool bFireOnlyOnce = false;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger",
            meta = (ClampMin = "0.0"))
  float ActivationDelay = 0.f;
};