// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIController.h"
#include "PokemonUtils.h"
#include "Pokemon.generated.h"

struct EXPLORATION_API FPokemonAnimationsSoftPtr
{
	TSoftObjectPtr<UAnimSequence> DefaultWait;

	TSoftObjectPtr<UAnimSequence> BattleWait;

	TSoftObjectPtr<UAnimSequence> DefaultIdle1;

	TSoftObjectPtr<UAnimSequence> DefaultIdle2;

	TSoftObjectPtr<UAnimSequence> Walk;

	TSoftObjectPtr<UAnimSequence> Run;

	TSoftObjectPtr<UAnimSequence> WildBoolStart;

	TSoftObjectPtr<UAnimSequence> WildBoolLoop;

	TSoftObjectPtr<UAnimSequence> WildBoolEnd;

	TSoftObjectPtr<UAnimSequence> TurnLeft;

	TSoftObjectPtr<UAnimSequence> TurnRight;

	TSoftObjectPtr<UAnimSequence> RestStart;

	TSoftObjectPtr<UAnimSequence> RestLoop;

	TSoftObjectPtr<UAnimSequence> RestEnd;

	TSoftObjectPtr<UAnimSequence> SleepStart;

	TSoftObjectPtr<UAnimSequence> SleepLoop;

	TSoftObjectPtr<UAnimSequence> SleepEnd;

	TSoftObjectPtr<UAnimSequence> Roar;

	TSoftObjectPtr<UAnimSequence> WildShot;

	TSoftObjectPtr<UAnimSequence> Attack1;

	TSoftObjectPtr<UAnimSequence> Attack2;

	TSoftObjectPtr<UAnimSequence> RangeAttack1;

	TSoftObjectPtr<UAnimSequence> RangeAttack2Start;

	TSoftObjectPtr<UAnimSequence> RangeAttack2Loop;

	TSoftObjectPtr<UAnimSequence> RangeAttack2End;

	TSoftObjectPtr<UAnimSequence> Damage1;

	TSoftObjectPtr<UAnimSequence> Damage2;

	TSoftObjectPtr<UAnimSequence> Glad;

	TSoftObjectPtr<UAnimSequence> Notice;

	TSoftObjectPtr<UAnimSequence> Hate;

	TSoftObjectPtr<UAnimSequence> UniqueWaitStart;

	TSoftObjectPtr<UAnimSequence> UniqueWaitLoop;

	TSoftObjectPtr<UAnimSequence> UniqueWaitEnd;
};

USTRUCT(BlueprintType)
struct EXPLORATION_API FPokemonAnimations
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* DefaultWait;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* BattleWait;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* DefaultIdle1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* DefaultIdle2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Walk;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Run;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* WildBoolStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* WildBoolLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* WildBoolEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* TurnLeft;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* TurnRight;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RestStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RestLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RestEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* SleepStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* SleepLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* SleepEnd;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Roar;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* WildShot;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Attack1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Attack2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RangeAttack1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RangeAttack2Start;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RangeAttack2Loop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* RangeAttack2End;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Damage1;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Damage2;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Glad;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Notice;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* Hate;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* UniqueWaitStart;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* UniqueWaitLoop;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	UAnimSequence* UniqueWaitEnd;
};

UCLASS()
class EXPLORATION_API APokemon : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	APokemon();

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ---------------
	// Custom Events
	// ---------------
	UFUNCTION(BlueprintImplementableEvent, Category = "Pokemon")
	void OnInitialize();

	void OnWalkingAnimationsLoaded();
	void OnSwimmingAnimationsLoaded();
	void OnFlyingAnimationsLoaded();

	// ---------------
	// Functions
	// ---------------
	UFUNCTION(BlueprintCallable, meta = (Category = "Pokemon"))
	void Initialize(const int32 entry);

	UFUNCTION(BlueprintCallable, meta = (Category = "Pokemon"))
	bool CanWalk() const { return (_allowedMoveTypes & static_cast<int32>(EPokemonMoveType::Walk)) != 0; }

	UFUNCTION(BlueprintCallable, meta = (Category = "Pokemon"))
	bool CanSwim() const { return (_allowedMoveTypes & static_cast<int32>(EPokemonMoveType::Swim)) != 0; }

	UFUNCTION(BlueprintCallable, meta = (Category = "Pokemon"))
	bool CanFly() const { return (_allowedMoveTypes & static_cast<int32>(EPokemonMoveType::Fly)) != 0; }

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	void InitializeAnimations(const int32 entry, const FString folderStr);

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	int32 _entry;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Pokemon")
	USoundBase* _crySound;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	FVector _speed;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	bool _isRunning;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	bool _isSleeping;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	FPokemonAnimations _pokemonAnimations;
	FPokemonAnimationsSoftPtr walkingAnimationsStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	FPokemonAnimations _pokemonAnimationsSwim;
	FPokemonAnimationsSoftPtr swimmingAnimationsStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	FPokemonAnimations _pokemonAnimationsFly;
	FPokemonAnimationsSoftPtr flyingAnimationsStruct;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	EPokemonMoveType _currentMoveTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, meta = (Category = "Pokemon", Bitmask, BitmaskEnum = EPokemonMoveType))
	int32 _allowedMoveTypes;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Pokemon")
	bool _showDebug;

	bool _areWalkingAnimationsLoaded;
	bool _areSwimmingAnimationsLoaded;
	bool _areFlyingAnimationsLoaded;
};
