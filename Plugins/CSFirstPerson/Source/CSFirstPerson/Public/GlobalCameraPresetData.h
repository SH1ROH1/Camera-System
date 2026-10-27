
#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GlobalCameraState.h"

#include "GlobalCameraPresetData.generated.h"


USTRUCT(BlueprintType)
struct FGlobalCameraEventerPreset
{
	GENERATED_BODY()

	// Preset.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Preset")
	FName PresetName;

	// Camera.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FGlobalCameraState CameraState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	FName ConnectTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	EGlobalCameraTransitionMode TransitionMode = EGlobalCameraTransitionMode::Smooth;

	// Path.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	FName CameraPathActorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path", meta = (ClampMin = "0.0"))
	float StartDistanceThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	float PathOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path", meta = (ClampMin = "0.1"))
	float FollowSmoothing = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Path")
	bool bPathLoop = false;

	// Trigger.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore")
	bool bRestoreOnExit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore")
	FName RestorePresetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore")
	FGlobalCameraState RestoreCameraState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore")
	FName RestoreConnectTargetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore")
	EGlobalCameraTransitionMode RestoreTransitionMode = EGlobalCameraTransitionMode::Smooth;

	// Restore Path.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore | Path")
	FName RestoreCameraPathActorName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore | Path", meta = (ClampMin = "0.0"))
	float RestoreStartDistanceThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore | Path")
	float RestorePathOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore | Path", meta = (ClampMin = "0.1"))
	float RestoreFollowSmoothing = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger | Restore | Path")
	bool bRestorePathLoop = false;

	// Trigger Settings.

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger")
	bool bFireOnlyOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Trigger", meta = (ClampMin = "0.0"))
	float ActivationDelay = 0.f;
};

UCLASS()
class CSFIRSTPERSON_API UGlobalCameraPresetData : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TArray<FGlobalCameraEventerPreset> Presets;


	bool FindPreset(FName PresetName, FGlobalCameraEventerPreset& OutPreset) const;
	bool SavePreset(FName PresetName, const FGlobalCameraEventerPreset& Preset, bool bOverwrite = false);
	bool DeletePreset(FName PresetName);
};