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
	//Expose(Blocks[curExpose]);
	bExpose = true;
}

bool ATetraminoActor::notExposed()
{
	for (ABlock* CurBlock : Blocks)
	{
		for (ABlock* AnotherBlock : Blocks)
		{
			if (CurBlock != AnotherBlock && UKismetMathLibrary::EqualEqual_VectorVector(CurBlock->GetActorLocation(),
				AnotherBlock->GetActorLocation(), 0.1))
			{
				UE_LOG(LogTemp, Log, TEXT("Not exposed! "));
				return true;
			}
		}
	}
	return false;
}

// Called every frame
void ATetraminoActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (bExpose)
	{
		bExpose = false;
		ABlock* IntersectBlock;
		for (ABlock* Block : Blocks)
		{
			if (!CheckAllExposed(Block, IntersectBlock))
			{
				IntersectBlock->Destination = IntersectBlock->GetActorLocation() +
				(UKismetMathLibrary::RandomBool()
					 ? FVector(0.0, 100.0, 0.0)
					 : UKismetMathLibrary::RandomBool()
					 ? FVector(0.0, -100.0, 0.0)
					 : UKismetMathLibrary::RandomBool()
					 ? FVector(100.0, 0.0, 0.0)
					 : FVector(-100.0, 0.0, 0.0));
				if (!IntersectBlock->ExposeDelegate.IsBound())
				{
					IntersectBlock->ExposeDelegate.BindLambda([&] { this->bExpose = true; });
				}
				IntersectBlock->bExposed = true;
				IntersectBlock->bShouldMove = true;
				return;
			}
		}
		bMove = true;
		this->movingBlocks = 4;
		UE_LOG(LogTemp, Log, TEXT("Expose all"));
	}
	if (bFall)
	{
		float Dist = -1.0f;
		bool bFallRes = true;
		for (ABlock* Block : Blocks)
		{
			Block->TraceLocation = Block->GetActorLocation();
			float D = Block->CalcDistance();
			if (!FMath::IsNearlyEqual(D, -1.0f, 0.1f))
			{
				Dist = D;
			}
			else
			{
				float CurDist = Block->CalcDistance();
				/*if (FMath::IsNearlyEqual(Dist, CurDist, 0.1f))
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
				}*/
			}
		}
		if (bFallRes)
		{
			for (ABlock* Block : Blocks)
			{
				FVector Loc = Block->GetActorLocation();
				Block->Destination = FVector(Loc.X, Loc.Y, Loc.Z - Dist + 50);
				this->fallingBlocks = 4;
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
			UE_LOG(LogTemp, Log, TEXT("Coords %s "), *Blocks[0]->GetActorLocation().ToString());
			FallDelegate.Broadcast(Blocks[0]->GetActorLocation().Z / 100);
		}
	}
	if (bMove)
	{
		FVector2D Corner =  UKismetMathLibrary::RandomBool()
			                   ? TopLeft
			                   : UKismetMathLibrary::RandomBool()
			                   ? TopRight
			                   : UKismetMathLibrary::RandomBool()
			                   ? BottomLeft
			                   : BottomRight; 
		/*FVector MovingVector = (UKismetMathLibrary::RandomBool()
			                        ? UKismetMathLibrary::RandomBool()
				                          ? FVector(0, 100, 0)
				                          : FVector(100, 0, 0)
			                        : UKismetMathLibrary::RandomBool()
			                        ? FVector(0, -100, 0)
			                        : FVector(-100, 0, 0));*/

		FVector DestLocation;

		// calculate place location from Bottom Left
		//profiled
		if (UKismetMathLibrary::EqualEqual_Vector2DVector2D(Corner, BottomLeft, 0.1))
		{
			UE_LOG(LogTemp, Log, TEXT("Place to BottomLeft"));
			//calculate lowest location
			FVector2D LowestLocation = TopRight;
			TArray<FVector2D> Coords;
			for (ABlock* Block1 : Blocks)
			{
				for (ABlock* Block2 : Blocks)
				{
					Coords.Add(FVector2D(Block1->GetActorLocation().X, Block2->GetActorLocation().Y));
				}
			}
			float DistToCorner = 800.0;
			for (FVector2D Loc : Coords)
			{
				float curDist = (BottomLeft - Loc).Size();
				if (curDist < DistToCorner)
				{
					DistToCorner = curDist;
					LowestLocation = Loc;
				}
			}

			UE_LOG(LogTemp, Log, TEXT("LowestLocation %s "), *LowestLocation.ToString());

			float DeepestPit = 0.0;
			for (int i = 0; i < 8; i++)
			{
				for (int k = 0; k < 8; k++)
				{
					float Dist = -1.0f;
					bool bFallRes = true;
					for (ABlock* Block : Blocks)
					{
						Block->TraceLocation = Block->GetActorLocation()
							+ FVector(100.0 * i, 100.0 * k, 0) - UKismetMathLibrary::Conv_Vector2DToVector(
								LowestLocation);
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
							}
							else
							{
								bFallRes = false;
								break;
							}
						}
					}
					if (bFallRes)
					{
						if (Dist > DeepestPit)
						{
							DeepestPit = Dist;
							DestLocation = FVector(100.0 * i, 100.0 * k, 0) - UKismetMathLibrary::Conv_Vector2DToVector(
								LowestLocation);
						}
					}
				}
			}
		}

		// calculate place location from Bottom  Right
		// profiled
		if (UKismetMathLibrary::EqualEqual_Vector2DVector2D(Corner, BottomRight, 0.1))
		{
			//calculate closest location
			UE_LOG(LogTemp, Log, TEXT("Place to BottomRight"));
			FVector2D LowestLocation = TopLeft;
			TArray<FVector2D> Coords;
			for (ABlock* Block1 : Blocks)
			{
				for (ABlock* Block2 : Blocks)
				{
					Coords.Add(FVector2D(Block1->GetActorLocation().X, Block2->GetActorLocation().Y));
				}
			}
			float DistToCorner = 800.0;
			for (FVector2D Loc : Coords)
			{
				float curDist = (BottomRight - Loc).Size();
				if (curDist < DistToCorner)
				{
					DistToCorner = curDist;
					LowestLocation = Loc;
				}
			}
			UE_LOG(LogTemp, Log, TEXT("LowestLocation %s "), *LowestLocation.ToString());
			float DeepestPit = 0.0;
			for (int i = 0; i < 8; i++)
			{
				for (int k = 0; k < 8; k++)
				{
					float Dist = -1.0f;
					bool bFallRes = true;
					for (ABlock* Block : Blocks)
					{
						Block->TraceLocation = Block->GetActorLocation() + FVector(100.0 * i, -100.0 * k, 0)
							+ FVector(-LowestLocation.X, LowestLocation.Y, 0);
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
							}
							else
							{
								bFallRes = false;
								break;
							}
						}
					}
					if (bFallRes)
					{
						if (Dist > DeepestPit)
						{
							DeepestPit = Dist;
							DestLocation = FVector(100.0 * i, -100.0 * k, 0)
								+ FVector(-LowestLocation.X, LowestLocation.Y, 0);
						}
					}
				}
			}
		}


		// calculate place location from Top  Left
		//profiled
		if (UKismetMathLibrary::EqualEqual_Vector2DVector2D(Corner, TopLeft, 0.1))
		{
			//calculate closest location
			UE_LOG(LogTemp, Log, TEXT("Place to TopLeft"));
			FVector2D LowestLocation = BottomRight;
			TArray<FVector2D> Coords;
			for (ABlock* Block1 : Blocks)
			{
				for (ABlock* Block2 : Blocks)
				{
					Coords.Add(FVector2D(Block1->GetActorLocation().X, Block2->GetActorLocation().Y));
				}
			}
			float DistToCorner = 800.0;
			for (FVector2D Loc : Coords)
			{
				float curDist = (TopLeft - Loc).Size();
				if (curDist < DistToCorner)
				{
					DistToCorner = curDist;
					LowestLocation = Loc;
				}
			}

			UE_LOG(LogTemp, Log, TEXT("LowestLocation %s "), *LowestLocation.ToString());
			float DeepestPit = 0.0;
			for (int i = 8; i > 0; i--)
			{
				for (int k = 0; k < 8; k++)
				{
					float Dist = -1.0f;
					bool bFallRes = true;
					for (ABlock* Block : Blocks)
					{
						Block->TraceLocation = Block->GetActorLocation()
							+ FVector(-100.0 * i, 100.0 * k, 0)
							+ FVector(LowestLocation.X, -LowestLocation.Y, 0);
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
							}
							else
							{
								bFallRes = false;
								break;
							}
						}
					}
					if (bFallRes)
					{
						if (Dist > DeepestPit)
						{
							DeepestPit = Dist;
							DestLocation = FVector(-100.0 * i, 100.0 * k, 0)
								+ FVector(LowestLocation.X, -LowestLocation.Y, 0);
						}
					}
				}
			}
		}

		// calculate place location from Top  Right
		//profiled
		if (UKismetMathLibrary::EqualEqual_Vector2DVector2D(Corner, TopRight, 0.1))
		{
			UE_LOG(LogTemp, Log, TEXT("Place to TopRight"));
			//calculate closest location
			FVector2D LowestLocation = BottomLeft;
			TArray<FVector2D> Coords;
			for (ABlock* Block1 : Blocks)
			{
				for (ABlock* Block2 : Blocks)
				{
					Coords.Add(FVector2D(Block1->GetActorLocation().X, Block2->GetActorLocation().Y));
				}
			}
			float DistToCorner = 800.0;
			for (FVector2D Loc : Coords)
			{
				float curDist = (TopRight - Loc).Size();
				if (curDist < DistToCorner)
				{
					DistToCorner = curDist;
					LowestLocation = Loc;
				}
			}

			UE_LOG(LogTemp, Log, TEXT("LowestLocation %s "), *LowestLocation.ToString());

			float DeepestPit = 0.0;
			for (int i = 8; i > 0; i--)
			{
				for (int k = 8; k > 0; k--)
				{
					float Dist = -1.0f;
					bool bFallRes = true;
					for (ABlock* Block : Blocks)
					{
						Block->TraceLocation = Block->GetActorLocation()
							+ FVector(100.0 * i, 100.0 * k, 0) - UKismetMathLibrary::Conv_Vector2DToVector(
								LowestLocation);
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
							}
							else
							{
								bFallRes = false;
								break;
							}
						}
					}
					if (bFallRes)
					{
						if (Dist > DeepestPit)
						{
							DeepestPit = Dist;
							DestLocation = FVector(100.0 * i, 100.0 * k, 0) - UKismetMathLibrary::Conv_Vector2DToVector(
								LowestLocation);
						}
					}
				}
			}
		}

		UE_LOG(LogTemp, Log, TEXT("Tetramino move!!! to %s "), *DestLocation.ToString());
		bMove = false;
		bMoving = true;
		for (ABlock* Block : Blocks)
		{
			Block->Destination = Block->GetActorLocation() + DestLocation;
			Block->bMoving = true;
			if (!Block->BlockMoveDelegate.IsBound())
			{
				Block->BlockMoveDelegate.BindLambda([&] { this->movingBlocks--; });
			}
			Block->bShouldMove = true;
		}
		
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
		Block->idx = i;
		Block->GetStaticMeshComponent()->SetMaterial(
			0, Materials[UKismetMathLibrary::RandomIntegerInRange(0, Materials.Num() - 1)]);
		Block->FallSpeed = Block->FallSpeed * speedMod;
		Block->HitActorDelegate.BindUObject(this, &ATetraminoActor::ReportHitActor);
		Blocks.Add(Block);
	}
}


bool ATetraminoActor::CheckAllExposed(ABlock* CurBlock, ABlock*& IntersectBlock)
{
	for (ABlock* AnotherBlock : Blocks)
	{
		if (CurBlock != AnotherBlock && UKismetMathLibrary::EqualEqual_VectorVector(CurBlock->GetActorLocation(),
			AnotherBlock->GetActorLocation(), 1))
		{
			IntersectBlock = AnotherBlock;
			return false;
		}
	}

	return true;
}

void ATetraminoActor::ReportHitActor()
{
	HitActorTetraminoDelegate.Broadcast();
}
