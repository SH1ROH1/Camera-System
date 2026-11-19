#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "GlobalCameraState.h"
#include "CameraTargetComponent.generated.h"

UCLASS(ClassGroup = (Camera), meta = (BlueprintSpawnableComponent))
class CSUNIVERSAL_API UCameraTargetComponent : public UActorComponent
{
	GENERATED_BODY()

protected:
	virtual void BeginPlay() override;

public:
	UCameraTargetComponent();

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Target")
	TObjectPtr<AActor> PlayerTarget;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera|Target")
	TObjectPtr<AActor> ConnectTarget;

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetPlayerTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Camera|Target")
	AActor* GetPlayerTarget() const;

	UFUNCTION(BlueprintCallable, Category = "Camera|Target")
	void SetConnectTarget(AActor* NewTarget);

	UFUNCTION(BlueprintPure, Category = "Camera|Target")
	AActor* GetConnectTarget() const;

	UFUNCTION(BlueprintPure, Category = "Camera|Target")
	AActor* GetTarget() const;

	UFUNCTION(BlueprintPure, Category = "Camera|Target")
	AActor* GetPositionTarget(EGlobalCameraPositionMode PositionMode) const;

	UFUNCTION(BlueprintPure, Category = "Camera|Target")
	AActor* GetFollowTarget(EGlobalCameraFollowTarget FollowTargetMode) const;

	UFUNCTION(BlueprintPure, Category = "Camera|Target")
	AActor* GetRotationTarget(
		EGlobalCameraFollowTarget FollowTargetMode,
		bool bUseTargetRotation
	) const;
};