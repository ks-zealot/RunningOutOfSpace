// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Block.generated.h"

/**
 * 
 */
DECLARE_DELEGATE(FExposeDelegate)
DECLARE_DELEGATE(FHitActorDelegate)
DECLARE_DELEGATE(FBlockFallDelegate)
DECLARE_DELEGATE(FBlockMoveDelegate)
DECLARE_DELEGATE(FExposeCompleteDelegate)
UCLASS()
class RUNNINGOUTOFSPACE_API ABlock : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ABlock();
	UFUNCTION(BlueprintCallable)
	void HitActorReport();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldMove = false;
	virtual void Tick(float DeltaSeconds) override;
	FVector Destination;
	FVector TraceLocation;
	FExposeDelegate ExposeDelegate;
	FExposeCompleteDelegate ExposeCompleteDelegate;
	FBlockFallDelegate BlockFallDelegate;
	FBlockMoveDelegate BlockMoveDelegate;
	FHitActorDelegate  HitActorDelegate;
	bool bExposed = false;
	bool bFalling = false;
	bool bMoving = false;
	int32 idx = 0;
	float CalcDistance();
	float ExposeSpeed = 20.0;
	float MoveSpeed = 5.0;
	float FallSpeed = 5.0;
protected:
	virtual void BeginPlay() override;
};
