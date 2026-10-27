#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Engine/TriggerVolume.h"
#include "SystemCamera.h"
#include "GlobalCameraPresetData.h"
#include "SystemCameraEventer.generated.h"

class USplineComponent;

UCLASS(Blueprintable)
class CSFIRSTPERSON_API ASystemCameraEventer : public ATriggerVolume
{
	GENERATED_BODY()

public:
	ASystemCameraEventer();

protected:
	virtual void BeginPlay() override;
	virtual void Tick(float DeltaTime) override;

	UFUNCTION()
	void OnActorEntered(AActor* OverlappedActor, AActor* OtherActor);

	UFUNCTION()
	void OnActorLeft(AActor* OverlappedActor, AActor* OtherActor);

public:

	//==========================================================
	// State | Presets
	//==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	FName PresetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Presets")
	TObjectPtr<UGlobalCameraPresetData> PresetData = nullptr;



#if WITH_EDITOR

	UFUNCTION(CallInEditor, Category = "Presets")
	void SavePreset();

	UFUNCTION(CallInEditor, Category = "Presets")
	void LoadPreset();

	UFUNCTION(CallInEditor, Category = "Presets")
	void DeletePreset();

#endif

	//==========================================================
	// State | Camera
	//==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Camera")
	FGlobalCameraState TargetState;



	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Camera")
	TObjectPtr<AActor> ConnectTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Camera")
	EGlobalCameraTransitionMode TransitionMode = EGlobalCameraTransitionMode::Smooth;

	//==========================================================
	// State | Path
	//==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Path")
	TObjectPtr<AActor> CameraPathActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Path", meta = (ClampMin = "0.0"))
	float StartDistanceThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Path")
	float PathOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Path", meta = (ClampMin = "0.1"))
	float FollowSmoothing = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Path")
	bool bPathLoop = false;

	//==========================================================
	// State | Trigger
	//==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore")
	bool bRestoreOnExit = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore", meta = (EditCondition = "bRestoreOnExit"))
	FGlobalCameraState RestoreTargetState;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore", meta = (EditCondition = "bRestoreOnExit"))
	FName RestorePresetName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore", meta = (EditCondition = "bRestoreOnExit"))
	TObjectPtr<AActor> RestoreConnectTarget = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore", meta = (EditCondition = "bRestoreOnExit"))
	EGlobalCameraTransitionMode RestoreTransitionMode = EGlobalCameraTransitionMode::Smooth;

	//==========================================================
	// State | Restore Path
	//==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore Path", meta = (EditCondition = "bRestoreOnExit"))
	TObjectPtr<AActor> RestoreCameraPathActor = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore Path", meta = (EditCondition = "bRestoreOnExit", ClampMin = "0.0"))
	float RestoreStartDistanceThreshold = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore Path", meta = (EditCondition = "bRestoreOnExit"))
	float RestorePathOffset = 0.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore Path", meta = (EditCondition = "bRestoreOnExit", ClampMin = "0.1"))
	float RestoreFollowSmoothing = 5.f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Restore Path", meta = (EditCondition = "bRestoreOnExit"))
	bool bRestorePathLoop = false;

	//==========================================================
	// Trigger
	//==========================================================

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Trigger")
	bool bFireOnlyOnce = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "State | Trigger", meta = (ClampMin = "0.0"))
	float ActivationDelay = 0.f;

	UPROPERTY(Transient)
	float ActivationTimer = 0.f;
	UPROPERTY(Transient)
	TWeakObjectPtr<AActor> PendingActor;
private:

	//==========================================================
	// Camera
	//==========================================================

	UPROPERTY()
	TObjectPtr<ASystemCamera> GameCamera = nullptr;

	//==========================================================
	// Path
	//==========================================================

	UPROPERTY()
	TObjectPtr<USplineComponent> ActiveSpline = nullptr;

	bool bTriggered = false;
	bool bPathActive = false;
	bool bPathFinished = false;

	float CurrentPathDistance = 0.f;

	//==========================================================
	// Path Settings
	//==========================================================

	float ActiveStartDistanceThreshold = 0.f;
	float ActivePathOffset = 0.f;
	float ActiveFollowSmoothing = 5.f;
	bool bActivePathLoop = false;

	//==========================================================
	// Functions
	//==========================================================

	void CheckExistingOverlap();

	void ApplyCameraSettings();

	void ActivateEventer(AActor* OtherActor);

	void StartPath();

	void UpdatePath(float DeltaTime);

	void StopPath();

	bool FindClosestPathDistance(float& OutDistance) const;

	void ApplyTargets(AActor* NewConnectTarget);

	void ApplyPathRotation();

#if WITH_EDITOR

	bool ConfirmOverwrite() const;

	AActor* FindActorByName(const FName& ActorName) const;

#endif
};