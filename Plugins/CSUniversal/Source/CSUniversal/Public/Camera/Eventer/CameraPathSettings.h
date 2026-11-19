#pragma once

#include "CoreMinimal.h"

#include "CameraPathSettings.generated.h"
USTRUCT(BlueprintType)
struct FCameraPathSettings {
  GENERATED_BODY()

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path",
            meta = (ClampMin = "0.0"))
  float StartDistanceThreshold = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  float PathOffset = 0.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path",
            meta = (ClampMin = "0.1"))
  float FollowSmoothing = 5.f;

  UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
  bool bLoop = false;
};