// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMeshActor.h"
#include "Block.generated.h"

/**
 * 
 */
DECLARE_DELEGATE_OneParam(FExposeDelegate, ABlock*)
DECLARE_DELEGATE(FBlockFallDelegate)
DECLARE_DELEGATE(FBlockMoveDelegate)
UCLASS()
class RUNNINGOUTOFSPACE_API ABlock : public AStaticMeshActor
{
	GENERATED_BODY()

public:
	ABlock();
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	bool bShouldMove = false;
	virtual void Tick(float DeltaSeconds) override;
	FVector Destination;
	FVector TraceLocation;
	FExposeDelegate ExposeDelegate;
	FBlockFallDelegate BlockFallDelegate;
	FBlockMoveDelegate BlockMoveDelegate;
	bool bExposed = false;
	bool bFalling = false;
	bool bMoving = false;
	int32 idx = 0;
	float CalcDistance();
	float ExposeSpeed = 10.0;
	float MoveSpeed = 10.0;
	float FallSpeed = 5.0;
protected:
	virtual void BeginPlay() override;
};
