// Fill out your copyright notice in the Description page of Project Settings.


#include "Pokemon.h"
#include "PokemonUtils.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include <format>
#include <string>

#define LOAD_ANIM(animationsStruct, animationName, animTier) (animationsStruct.animationName = loadAnimation (EPokemonAnimations::animationName, animTier))

#define LOAD_ANIMS(animationStruct, tier) \
    LOAD_ANIM(animationStruct, DefaultWait, tier); \
    LOAD_ANIM(animationStruct, BattleWait, tier); \
    LOAD_ANIM(animationStruct, DefaultIdle1, tier); \
    LOAD_ANIM(animationStruct, DefaultIdle2, tier); \
    LOAD_ANIM(animationStruct, TurnLeft, tier); \
    LOAD_ANIM(animationStruct, TurnRight, tier); \
    LOAD_ANIM(animationStruct, Walk, tier); \
    LOAD_ANIM(animationStruct, Run, tier); \
    LOAD_ANIM(animationStruct, WildBoolStart, tier); \
    LOAD_ANIM(animationStruct, WildBoolLoop, tier); \
    LOAD_ANIM(animationStruct, WildBoolEnd, tier); \
    LOAD_ANIM(animationStruct, RestStart, tier); \
    LOAD_ANIM(animationStruct, RestLoop, tier); \
    LOAD_ANIM(animationStruct, RestEnd, tier); \
    LOAD_ANIM(animationStruct, SleepStart, tier); \
    LOAD_ANIM(animationStruct, SleepLoop, tier); \
    LOAD_ANIM(animationStruct, SleepEnd, tier); \
    LOAD_ANIM(animationStruct, Roar, tier); \
    LOAD_ANIM(animationStruct, WildShot, tier); \
    LOAD_ANIM(animationStruct, Attack1, tier); \
    LOAD_ANIM(animationStruct, Attack2, tier); \
    LOAD_ANIM(animationStruct, RangeAttack1, tier); \
    LOAD_ANIM(animationStruct, RangeAttack2Start, tier); \
    LOAD_ANIM(animationStruct, RangeAttack2Loop, tier); \
    LOAD_ANIM(animationStruct, RangeAttack2End, tier); \
    LOAD_ANIM(animationStruct, Damage1, tier); \
    LOAD_ANIM(animationStruct, Damage2, tier); \
    LOAD_ANIM(animationStruct, Glad, tier); \
    LOAD_ANIM(animationStruct, Notice, tier); \
    LOAD_ANIM(animationStruct, Hate, tier); \
    LOAD_ANIM(animationStruct, UniqueWaitStart, tier); \
    LOAD_ANIM(animationStruct, UniqueWaitLoop, tier); \
    LOAD_ANIM(animationStruct, UniqueWaitEnd, tier)

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
        LOAD_ANIMS(_pokemonAnimations, animTier);
    }

    animTier = EPokemonAnimTier::Swimming;
    if (CanSwim())
    {
        LOAD_ANIMS(_pokemonAnimationsSwim, animTier);
    }

    animTier = EPokemonAnimTier::Flying;
    if (CanFly())
    {
        LOAD_ANIMS(_pokemonAnimationsFly, animTier);
    }
}

