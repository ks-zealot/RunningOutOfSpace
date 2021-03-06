// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Block.h"
#include "GameFramework/Actor.h"
#include "TetraminoActor.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFallDelegate, int32, level);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FHitActorTetraminoDelegate );
UCLASS()
class RUNNINGOUTOFSPACE_API ATetraminoActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ATetraminoActor();
	UPROPERTY(EditInstanceOnly,  BlueprintReadWrite, Meta= (ExposeOnSpawn=true))
	int32 speedMod =1;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TSubclassOf<class ABlock> BlockBlueprint;
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite)
	TArray <class UMaterial *> Materials;
	UPROPERTY(BlueprintAssignable)
	FFallDelegate FallDelegate;
	UPROPERTY(BlueprintAssignable)
	FHitActorTetraminoDelegate HitActorTetraminoDelegate;
	UFUNCTION(BlueprintCallable)
	void ReportHitActor();
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
private:
	TArray<ABlock*> Blocks;
	bool notExposed();
	void GenerateBlocks();
	bool CheckAllExposed(ABlock* Block, ABlock*& IntersectBlock);
	int32 curExpose = 0;
	bool bExpose = false;
	bool bFall = false;
	bool bFalling = false;
	bool bMove = false;
	bool bMoving = false;
	int32 fallingBlocks = 0;
	int32 movingBlocks = 0;
//corners
	
	FVector2D BottomLeft = FVector2D::ZeroVector;
	FVector2D TopLeft = FVector2D(800.0, 0.0  );
	FVector2D BottomRight =  FVector2D(0.0, 800.0 );
	FVector2D TopRight = FVector2D(800.0, 800.0);
};
