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
			Block->TraceLocation = Block->GetActorLocation();
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
		FVector2D Corner = UKismetMathLibrary::RandomBool()
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
			UE_LOG(LogTemp, Log, TEXT("Place to BottomLeft")   );
			//calculate lowest location
			FVector2D LowestLocation = TopRight;
			for (ABlock* Block : Blocks)
			{
				if (Block->GetActorLocation().X < LowestLocation.X
					&& Block->GetActorLocation().Y < LowestLocation.Y)
				{
					LowestLocation = FVector2D(Block->GetActorLocation());
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
								UE_LOG(LogTemp, Log, TEXT("could fall %f"), CurDist);
							}
							else
							{
								bFallRes = false;
								UE_LOG(LogTemp, Log, TEXT("could not fall %f"), CurDist);
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
			UE_LOG(LogTemp, Log, TEXT("Place to BottomRight")   );
			FVector2D LowestLocation = TopLeft;
			for (ABlock* Block : Blocks)
			{
				if (Block->GetActorLocation().X < LowestLocation.X
					&& Block->GetActorLocation().Y > LowestLocation.Y)
				{
					LowestLocation = FVector2D(Block->GetActorLocation());
				}
			}
			UE_LOG(LogTemp, Log, TEXT("LowestLocation %s "), *LowestLocation.ToString());
			float DeepestPit = 0.0;
			for (int k = 0; k < 8; k++)
			{
				for (int i = 0; i < 8; i++)
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
								UE_LOG(LogTemp, Log, TEXT("could fall %f"), CurDist);
							}
							else
							{
								bFallRes = false;
								UE_LOG(LogTemp, Log, TEXT("could not fall %f"), CurDist);
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
		if (UKismetMathLibrary::EqualEqual_Vector2DVector2D(Corner, TopLeft  , 0.1))
		{
			//calculate closest location
			UE_LOG(LogTemp, Log, TEXT("Place to TopLeft")   );
			FVector2D LowestLocation = BottomRight;
			for (ABlock* Block : Blocks)
			{
				if (Block->GetActorLocation().X > LowestLocation.X
					&& Block->GetActorLocation().Y < LowestLocation.Y)
				{
					LowestLocation = FVector2D(Block->GetActorLocation());
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
								UE_LOG(LogTemp, Log, TEXT("could fall %f"), CurDist);
							}
							else
							{
								bFallRes = false;
								UE_LOG(LogTemp, Log, TEXT("could not fall %f"), CurDist);
								break;
							}
						}
					}
					if (bFallRes)
					{
						if (Dist > DeepestPit)
						{
							DeepestPit = Dist;
							DestLocation =  FVector(-100.0 * i, 100.0 * k, 0)
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
			UE_LOG(LogTemp, Log, TEXT("Place to TopRight")   );
			//calculate closest location
			FVector2D LowestLocation = BottomLeft;
			for (ABlock* Block : Blocks)
			{
				if (Block->GetActorLocation().X > LowestLocation.X
					&& Block->GetActorLocation().Y > LowestLocation.Y)
				{
					LowestLocation = FVector2D(Block->GetActorLocation());
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
								UE_LOG(LogTemp, Log, TEXT("could fall %f"), CurDist);
							}
							else
							{
								bFallRes = false;
								UE_LOG(LogTemp, Log, TEXT("could not fall %f"), CurDist);
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
		/*for (ABlock* Block : Blocks)
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
		}*/
		//bMove = false;
		//bMoving = true;
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
			bMove = true;
			this->movingBlocks = 4;
			// bFall = true;
			// fallingBlocks = 4;
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
