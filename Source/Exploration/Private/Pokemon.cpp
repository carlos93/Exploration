// Fill out your copyright notice in the Description page of Project Settings.


#include "Pokemon.h"
#include "PokemonUtils.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Engine/AssetManager.h"
#include <format>
#include <string>


#define CREATE_SOFT_POINTER_ANIM(member, animId, tier, container) \
    member.animId = createPtr(EPokemonAnimations::animId, tier); \
    container.AddUnique(member.animId.ToSoftObjectPath());

#define CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, animId, tier, container) \
    CREATE_SOFT_POINTER_ANIM(member, animId##Start, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, animId##Loop, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, animId##End, tier, container);

#define FILL_CONTAINER_ANIMS(member, tier, container) \
    CREATE_SOFT_POINTER_ANIM(member, DefaultWait, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, BattleWait, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, DefaultIdle1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, DefaultIdle2, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, TurnLeft, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, TurnRight, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Walk, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Run, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, WildBool, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, Rest, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, Sleep, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Roar, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, WildShot, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Attack1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Attack2, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RangeAttack1, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, RangeAttack1, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, RangeAttack2, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Damage1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Damage2, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, Stun, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, Down, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Glad, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Notice, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Hate, tier, container); \
    CREATE_SOFT_POINTER_ANIM_START_LOOP_END(member, UniqueWait, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Eye, tier, container);

#define LOAD_ANIM(member, anim, container) member.anim = container.anim.LoadSynchronous();

#define LOAD_ANIM_START_LOOP_END(member, anim, container) \
    LOAD_ANIM(member, anim##Start, container); \
    LOAD_ANIM(member, anim##Loop, container); \
    LOAD_ANIM(member, anim##End, container);

#define LOAD_ANIMS(member, container) \
    LOAD_ANIM(member, DefaultWait , container); \
    LOAD_ANIM(member, BattleWait , container); \
    LOAD_ANIM(member, DefaultIdle1, container); \
    LOAD_ANIM(member, DefaultIdle2, container); \
    LOAD_ANIM(member, TurnLeft, container); \
    LOAD_ANIM(member, TurnRight, container); \
    LOAD_ANIM(member, Walk, container); \
    LOAD_ANIM(member, Run, container); \
    LOAD_ANIM_START_LOOP_END(member, WildBool, container); \
    LOAD_ANIM_START_LOOP_END(member, Rest, container); \
    LOAD_ANIM_START_LOOP_END(member, Sleep, container); \
    LOAD_ANIM(member, Roar, container); \
    LOAD_ANIM(member, WildShot, container); \
    LOAD_ANIM(member, Attack1, container); \
    LOAD_ANIM(member, Attack2, container); \
    LOAD_ANIM(member, RangeAttack1, container); \
    LOAD_ANIM_START_LOOP_END(member, RangeAttack1, container); \
    LOAD_ANIM_START_LOOP_END(member, RangeAttack2, container); \
    LOAD_ANIM(member, Damage1, container); \
    LOAD_ANIM(member, Damage2, container); \
    LOAD_ANIM(member, Glad, container); \
    LOAD_ANIM(member, Notice, container); \
    LOAD_ANIM(member, Hate, container); \
    LOAD_ANIM_START_LOOP_END(member, Stun, container); \
    LOAD_ANIM_START_LOOP_END(member, Down, container); \
    LOAD_ANIM_START_LOOP_END(member, UniqueWait, container); \
    LOAD_ANIM(member, Eye, container)

// Sets default values
APokemon::APokemon() : _entry{0}, _crySound(nullptr), _speed{}, _isRunning{false}, _isSleeping{false}, _pokemonAnimations{}, _pokemonAnimationsSwim{}, _pokemonAnimationsFly{}, _defaultMoveType{EPokemonMoveType::Walk}, _allowedMoveTypes{static_cast<int32>(EPokemonMoveType::Walk |EPokemonMoveType::Swim)}, _showDebug{false}, _areWalkingAnimationsLoaded{false}, _areSwimmingAnimationsLoaded{false}, _areFlyingAnimationsLoaded{false}
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

void APokemon::Initialize(const int32 entry, uint8 gender, uint8 form)
{
    UPokemonUtils::InitDatabase();

    _areWalkingAnimationsLoaded = false;
    _areSwimmingAnimationsLoaded = false;
    _areFlyingAnimationsLoaded = false;

    FString entryFixed = FString(std::format("{:0>4}", entry).c_str());
    FString genderStr = FString(std::format("{:0>2}", gender).c_str());
    FString formStr = FString(std::format("{:0>2}", form).c_str());

    FString folderStr = "/Game/Models/pm" + entryFixed + "_" + genderStr + "_" + formStr + "/";

    FString skeletalMeshPath = folderStr + "pm" + entryFixed + "_" + genderStr + "_" + formStr;
    USkeletalMesh* skeletalMesh = LoadObject<USkeletalMesh>(nullptr, *skeletalMeshPath);
    if (!skeletalMesh)
    {
        folderStr = "/Game/Models/pm" + entryFixed + "_00_00/";
        skeletalMeshPath = folderStr + "pm" + entryFixed + "_00_00";
        gender = 0U;
        skeletalMesh = LoadObject<USkeletalMesh>(nullptr, *skeletalMeshPath);
    }
    GetMesh()->SetSkeletalMesh(skeletalMesh);

    if (!CanWalk())
    {
        if (CanSwim())
        {
            _defaultMoveType = EPokemonMoveType::Swim;
        }
        else if (CanFly())
        {
            _defaultMoveType = EPokemonMoveType::Fly;
        }
    }

    auto createPtr = [&](const EPokemonAnimations pokemonAnimation, const EPokemonAnimTier pokemonAnimTier)
    {
        FSoftObjectPath animationPath = folderStr + UPokemonUtils::GetAnimationNameForPokemon(entry, pokemonAnimation, pokemonAnimTier, gender);
        TSoftObjectPtr<UAnimSequence> animationPtr(animationPath);
        return animationPtr;
    };

    FStreamableManager& Streamable = UAssetManager::GetStreamableManager();
    TArray<FSoftObjectPath> walkingAnimations;
    FPokemonAnimationsSoftPtr animationsStruct;
    auto animTier = EPokemonAnimTier::Normal;
    if (CanWalk())
    {
        FILL_CONTAINER_ANIMS(walkingAnimationsStruct, animTier, walkingAnimations);
        CREATE_SOFT_POINTER_ANIM(walkingAnimationsStruct, Eye, EPokemonAnimTier::Normal, walkingAnimations);
        Streamable.RequestAsyncLoad(walkingAnimations, FStreamableDelegate::CreateUObject(this, &APokemon::OnWalkingAnimationsLoaded));
    }
    else
    {
        _areWalkingAnimationsLoaded = true;
    }

    animTier = EPokemonAnimTier::Swimming;
    TArray<FSoftObjectPath> swimmingAnimations;
    if (CanSwim())
    {
        FILL_CONTAINER_ANIMS(swimmingAnimationsStruct, animTier, swimmingAnimations);
        CREATE_SOFT_POINTER_ANIM(swimmingAnimationsStruct, Eye, EPokemonAnimTier::Normal, swimmingAnimations);
        Streamable.RequestAsyncLoad(swimmingAnimations, FStreamableDelegate::CreateUObject(this, &APokemon::OnSwimmingAnimationsLoaded));
    }
    else
    {
        _areSwimmingAnimationsLoaded = true;
    }

    animTier = EPokemonAnimTier::Flying;
    TArray<FSoftObjectPath> flyingAnimations;
    if (CanFly())
    {
        FILL_CONTAINER_ANIMS(flyingAnimationsStruct, animTier, flyingAnimations);
        CREATE_SOFT_POINTER_ANIM(flyingAnimationsStruct, Eye, EPokemonAnimTier::Normal, flyingAnimations);
        Streamable.RequestAsyncLoad(flyingAnimations, FStreamableDelegate::CreateUObject(this, &APokemon::OnFlyingAnimationsLoaded));
    }
    else
    {
        _areFlyingAnimationsLoaded = true;
    }
}

void APokemon::OnWalkingAnimationsLoaded()
{
    _areWalkingAnimationsLoaded = true;

    LOAD_ANIMS(_pokemonAnimations, walkingAnimationsStruct);

    if (_pokemonAnimations.BattleWait == nullptr)
    {
        _pokemonAnimations.BattleWait = _pokemonAnimations.DefaultWait;
    }

    if (_areWalkingAnimationsLoaded && _areSwimmingAnimationsLoaded && _areFlyingAnimationsLoaded)
        OnInitialize();
}

void APokemon::OnSwimmingAnimationsLoaded()
{
    _areSwimmingAnimationsLoaded = true;

    LOAD_ANIMS(_pokemonAnimationsSwim, swimmingAnimationsStruct);

    if (_pokemonAnimationsSwim.BattleWait == nullptr)
    {
        _pokemonAnimationsSwim.BattleWait = _pokemonAnimationsSwim.DefaultWait;
    }

    if (_areWalkingAnimationsLoaded && _areSwimmingAnimationsLoaded && _areFlyingAnimationsLoaded)
        OnInitialize();
}

void APokemon::OnFlyingAnimationsLoaded()
{
    _areFlyingAnimationsLoaded = true;

    LOAD_ANIMS(_pokemonAnimationsFly, flyingAnimationsStruct);

    if (_pokemonAnimationsFly.BattleWait == nullptr)
    {
        _pokemonAnimationsFly.BattleWait = _pokemonAnimationsFly.DefaultWait;
    }

    if (_areWalkingAnimationsLoaded && _areSwimmingAnimationsLoaded && _areFlyingAnimationsLoaded)
        OnInitialize();
}

