// Fill out your copyright notice in the Description page of Project Settings.


#include "TetraminoActor.h"

#include "Kismet/KismetMathLibrary.h"

// Sets default values
ATetraminoActor::ATetraminoActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ATetraminoActor::BeginPlay()
{
	Super::BeginPlay();
	GenerateBlocks();
	Expose(Blocks[curExpose]);
}

// Called every frame
void ATetraminoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bFall)
	{
		float Dist = -1.0f;
		bool bFallRes = true;
		for (ABlock* Block : Blocks)
		{
			if (FMath::IsNearlyEqual(Dist, -1.0f, 0.1f))
			{
				Dist = Block->CalcDistance();
			}
			else
			{
				float CurDist = Block->CalcDistance();
				if (FMath::IsNearlyEqual(Dist, CurDist, 0.1f))
				{
					Dist = CurDist;
					UE_LOG(LogTemp, Log, TEXT("could fall %f"), CurDist);
				}
				else
				{
					bFallRes = false;
					bMove = true;
					this->movingBlocks = 4;
					UE_LOG(LogTemp, Log, TEXT("could not fall %f"), CurDist);
					break;
				}
			}
		}
		if (bFallRes)
		{
			for (ABlock* Block : Blocks)
			{
				FVector Loc = Block->GetActorLocation();
				Block->Destination = FVector(Loc.X, Loc.Y, Loc.Z - Dist + 50);
				Block->bFalling = true;
				Block->BlockFallDelegate.BindLambda([&] { this->fallingBlocks--; });
				Block->bShouldMove = true;
			}
		}
		bFall = false;
		bFalling = true;
	}
	if (bFalling)
	{
		if (fallingBlocks == 0)
		{
			bFalling = false;
			UE_LOG(LogTemp, Log, TEXT("Tetramino falls "));
			FallDelegate.Broadcast();
		}
	}
	if (bMove)
	{
		
		FVector MovingVector = (UKismetMathLibrary::RandomBool()  ?  UKismetMathLibrary::RandomBool() ? FVector(0, 100, 0) : FVector(100, 0, 0)
			:  UKismetMathLibrary::RandomBool() ? FVector(0, -100, 0) : FVector(-100, 0, 0));
		for (ABlock* Block : Blocks)
		{
			if (Block->bMoving)
			{
				UE_LOG(LogTemp, Log, TEXT("Block a;ready move ") );
				continue;
			}
			FVector Loc = Block->GetActorLocation();
			UE_LOG(LogTemp, Log, TEXT("Tetramino move to %s "), *Loc.ToString());
			Block->Destination = Loc + MovingVector;
			Block->bMoving = true;
			Block->BlockMoveDelegate.BindLambda([&] { this->movingBlocks--; });
			Block->bShouldMove = true;
		}
		bMove = false;
		bMoving = true;
	}
	if (bMoving)
	{
		if (movingBlocks == 0)
		{
			bMoving = false;
			bFall = true;
			UE_LOG(LogTemp, Log, TEXT("Tetramino move "));
		}
	}
}

void ATetraminoActor::GenerateBlocks()
{
	for (int32 i = 0; i < 4; i++)
	{
		const FTransform Transform =
			UKismetMathLibrary::MakeTransform(GetActorLocation(), FRotator::ZeroRotator,
			                                  FVector::OneVector);
		ABlock* Block = GetWorld()->SpawnActor<ABlock>(
			BlockBlueprint.Get(), Transform);
		Block->ExposeDelegate.BindUObject(this, &ATetraminoActor::Expose);
		Block->idx = i;
		Block->GetStaticMeshComponent()->SetMaterial(
			0, Materials[UKismetMathLibrary::RandomIntegerInRange(0, Materials.Num() - 1)]);
		Blocks.Add(Block);
	}
}

void ATetraminoActor::Expose(ABlock* Block)
{
	//i have no idea how its works
	UE_LOG(LogTemp, Log, TEXT("Expose  (). %d "), curExpose);
	ABlock* IntersectBlock;
	curExpose++;
	if (CheckAllExposed(Block, IntersectBlock))
	{
		curExpose++;
		if (curExpose < 4)
		{
			Expose(Blocks[curExpose]);
		}
		else
		{
			for (ABlock* B : Blocks)
			{
				if (!CheckAllExposed(B, IntersectBlock))
				{
					curExpose = 0;
					Expose(Blocks[curExpose]);
					break;
				}
			}
		}
		if (curExpose > 4)
		{
			UE_LOG(LogTemp, Log, TEXT("Stop Expose 1  (). %d "), curExpose);
			bFall = true;
			fallingBlocks = 4;
		}
		return;
	}
	IntersectBlock->Destination = IntersectBlock->GetActorLocation() +
		(UKismetMathLibrary::RandomBool() ? FVector(0, 100, 0) : FVector(100, 0, 0));
	IntersectBlock->bExposed = true;
	IntersectBlock->bShouldMove = true;
}

bool ATetraminoActor::CheckAllExposed(ABlock* CurBlock, ABlock*& IntersectBlock)
{
	for (ABlock* AnotherBlock : Blocks)
	{
		if (CurBlock != AnotherBlock && UKismetMathLibrary::EqualEqual_VectorVector(CurBlock->GetActorLocation(),
			AnotherBlock->GetActorLocation(), 0.1))
		{
			IntersectBlock = AnotherBlock;
			return false;
		}
	}

	return true;
}
