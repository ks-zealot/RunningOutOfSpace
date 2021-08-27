// Fill out your copyright notice in the Description page of Project Settings.


#include "Block.h"

#include "DrawDebugHelpers.h"
#include "Kismet/KismetMathLibrary.h"

ABlock::ABlock()
{
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
	 
}

void ABlock::BeginPlay()
{
	Super::BeginPlay();
	Destination = GetActorLocation();
}


void ABlock::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);
	if (bShouldMove)
	{
		SetActorLocation(UKismetMathLibrary::VLerp(GetActorLocation(), Destination, 0.1));
		if (UKismetMathLibrary::EqualEqual_VectorVector(GetActorLocation(), Destination, 0.1))
		{
			bShouldMove = false;
			if (bExposed)
			{
				bExposed = false;
				UE_LOG(LogTemp, Log, TEXT("Expose next."));
				ExposeDelegate.Execute(this);
			}
			if (bFalling)
			{
				bFalling = false;
				UE_LOG(LogTemp, Log, TEXT("notify fall."));
				BlockFallDelegate.Execute();
			}
			if (bMoving)
			{
				bMoving = false;
				UE_LOG(LogTemp, Log, TEXT("notify move."));
				BlockMoveDelegate.Execute();
			}
		}
		
	}
	
}

float ABlock::CalcDistance()
{
	FHitResult HitResult;
	FVector Start =GetActorLocation();
	FVector End =  FVector(GetActorLocation().X,  GetActorLocation().Y, 0);
	FCollisionQueryParams CollisionParams;
	CollisionParams.AddIgnoredActor(this);
	if (GetWorld()->LineTraceSingleByChannel(HitResult, Start, End, ECC_GameTraceChannel1, CollisionParams))
	{
		return HitResult.Distance;
	} else
	{
		return -1;
	}
}
