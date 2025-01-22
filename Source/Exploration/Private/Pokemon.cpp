// Fill out your copyright notice in the Description page of Project Settings.


#include "Pokemon.h"
#include "PokemonUtils.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include <format>
#include <string>

#define LOAD_ANIM(animationsStruct, animationName, animTier) (animationsStruct.animationName = loadAnimation (EPokemonAnimations::animationName, animTier))

// Sets default values
APokemon::APokemon() : _entry{0}, _crySound(nullptr), _speed{}, _isRunning{false}, _isSleeping{false}, _pokemonAnimations{}, _pokemonAnimationsSwim{}, _pokemonAnimationsFly{}, _currentMoveTypes{EPokemonMoveType::Walk}, _allowedMoveTypes{static_cast<int32>(EPokemonMoveType::Walk |EPokemonMoveType::Swim)}, _showDebug{false}
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void APokemon::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void APokemon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// Called to bind functionality to input
void APokemon::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void APokemon::Initialize(const int32 entry)
{
    UPokemonUtils::InitDatabase();

    FString entryStr = FString::FromInt(entry);
    FString entryFixed = FString(std::format("{:0>4}", entry).c_str());
    FString folderStr = "/Game/Models/pm" + entryFixed + "/";
    FString skeletalMeshPath = folderStr + "pm" + entryFixed;

    USkeletalMesh* skeletalMesh = LoadObject<USkeletalMesh>(nullptr, *skeletalMeshPath);
    GetMesh()->SetSkeletalMesh(skeletalMesh);

    if (!CanWalk())
    {
        if (CanSwim())
        {
            _currentMoveTypes = EPokemonMoveType::Swim;
        }
        else if (CanFly())
        {
            _currentMoveTypes = EPokemonMoveType::Fly;
        }
    }

    InitializeAnimations(entry, folderStr);

    FString cryPath = "/Game/Sounds/Cries/" + entryStr;
    _crySound = LoadObject<USoundBase>(nullptr, *cryPath);

    OnInitialize();
}

void APokemon::InitializeAnimations(const int32 entry, const FString folderStr)
{
    const EPokemonSpecies pokemonSpecies = static_cast<EPokemonSpecies>(entry);
    auto loadAnimation = [&](const EPokemonAnimations pokemonAnimation, const EPokemonAnimTier pokemonAnimTier)
    {
        FString animationPath = folderStr + UPokemonUtils::GetPathForPokemonAnimation(pokemonSpecies, pokemonAnimation, pokemonAnimTier);
        UAnimSequence* anim = LoadObject<UAnimSequence>(nullptr, *animationPath);
        if (!anim)
        {
            animationPath = folderStr + UPokemonUtils::GetPathForPokemonAnimation(pokemonSpecies, pokemonAnimation, pokemonAnimTier);
            anim = LoadObject<UAnimSequence>(nullptr, *animationPath);
        }

        if (!anim && _showDebug && GEngine)
        {
            FString errorText = L"Error while loading anim for species: " + FString::FromInt(static_cast<int32>(pokemonSpecies)) + L" animId: " + FString::FromInt(static_cast<int32>(pokemonAnimation)) + L" tierId: " + FString::FromInt(static_cast<int32>(pokemonAnimTier));
            GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, errorText);
        }
        return anim;
    };

    auto animTier = EPokemonAnimTier::Normal;
    if (CanWalk())
    {
        LOAD_ANIM(_pokemonAnimations, DefaultWait, animTier);
        LOAD_ANIM(_pokemonAnimations, BattleWait, animTier);
        LOAD_ANIM(_pokemonAnimations, DefaultIdle1, animTier);
        LOAD_ANIM(_pokemonAnimations, DefaultIdle2, animTier);
        LOAD_ANIM(_pokemonAnimations, TurnLeft, animTier);
        LOAD_ANIM(_pokemonAnimations, TurnRight, animTier);
        LOAD_ANIM(_pokemonAnimations, Walk, animTier);
        LOAD_ANIM(_pokemonAnimations, Run, animTier);
        LOAD_ANIM(_pokemonAnimations, WildBoolStart, animTier);
        LOAD_ANIM(_pokemonAnimations, WildBoolLoop, animTier);
        LOAD_ANIM(_pokemonAnimations, WildBoolEnd, animTier);
        LOAD_ANIM(_pokemonAnimations, RestStart, animTier);
        LOAD_ANIM(_pokemonAnimations, RestLoop, animTier);
        LOAD_ANIM(_pokemonAnimations, RestEnd, animTier);
        LOAD_ANIM(_pokemonAnimations, SleepStart, animTier);
        LOAD_ANIM(_pokemonAnimations, SleepLoop, animTier);
        LOAD_ANIM(_pokemonAnimations, SleepEnd, animTier);
        LOAD_ANIM(_pokemonAnimations, Roar, animTier);
        LOAD_ANIM(_pokemonAnimations, Attack1, animTier);
        LOAD_ANIM(_pokemonAnimations, Attack2, animTier);
        LOAD_ANIM(_pokemonAnimations, RangeAttack1, animTier);
        LOAD_ANIM(_pokemonAnimations, RangeAttack2Start, animTier);
        LOAD_ANIM(_pokemonAnimations, RangeAttack2Loop, animTier);
        LOAD_ANIM(_pokemonAnimations, RangeAttack2End, animTier);
        LOAD_ANIM(_pokemonAnimations, Damage1, animTier);
        LOAD_ANIM(_pokemonAnimations, Damage2, animTier);
        LOAD_ANIM(_pokemonAnimations, Glad, animTier);
        LOAD_ANIM(_pokemonAnimations, Notice, animTier);
        LOAD_ANIM(_pokemonAnimations, Hate, animTier);
        LOAD_ANIM(_pokemonAnimations, UniqueWaitStart, animTier);
        LOAD_ANIM(_pokemonAnimations, UniqueWaitLoop, animTier);
        LOAD_ANIM(_pokemonAnimations, UniqueWaitEnd, animTier);
    }

    animTier = EPokemonAnimTier::Swimming;
    if (CanSwim())
    {
        LOAD_ANIM(_pokemonAnimationsSwim, DefaultWait, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, BattleWait, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, DefaultIdle1, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, DefaultIdle2, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, TurnLeft, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, TurnRight, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Walk, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Run, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, WildBoolStart, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, WildBoolLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, WildBoolEnd, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RestStart, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RestLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RestEnd, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, SleepStart, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, SleepLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, SleepEnd, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Roar, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Attack1, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Attack2, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RangeAttack1, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RangeAttack2Start, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RangeAttack2Loop, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, RangeAttack2End, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Damage1, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Damage2, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Glad, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Notice, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, Hate, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, UniqueWaitStart, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, UniqueWaitLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsSwim, UniqueWaitEnd, animTier);
    }

    animTier = EPokemonAnimTier::Flying;
    if (CanFly())
    {
        LOAD_ANIM(_pokemonAnimationsFly, DefaultWait, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, BattleWait, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, DefaultIdle1, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, DefaultIdle2, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, TurnLeft, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, TurnRight, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Walk, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Run, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, WildBoolStart, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, WildBoolLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, WildBoolEnd, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RestStart, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RestLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RestEnd, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, SleepStart, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, SleepLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, SleepEnd, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Roar, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Attack1, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Attack2, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RangeAttack1, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RangeAttack2Start, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RangeAttack2Loop, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, RangeAttack2End, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Damage1, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Damage2, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Glad, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Notice, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, Hate, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, UniqueWaitStart, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, UniqueWaitLoop, animTier);
        LOAD_ANIM(_pokemonAnimationsFly, UniqueWaitEnd, animTier);
    }
}

