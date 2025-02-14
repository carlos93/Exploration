// Fill out your copyright notice in the Description page of Project Settings.


#include "Pokemon.h"
#include "PokemonUtils.h"
#include "Animation/AnimInstance.h"
#include "Animation/AnimNode_StateMachine.h"
#include "Engine/AssetManager.h"
#include <format>
#include <string>

//#define LOAD_ANIM(animationsStruct, animationName, animTier) (animationsStruct.animationName = loadAnimation (EPokemonAnimations::animationName, animTier))


#define CREATE_SOFT_POINTER_ANIM(member, animId, tier, container) \
    member.animId = createPtr(EPokemonAnimations::animId, tier); \
    container.AddUnique(member.animId.ToSoftObjectPath());

#define FILL_CONTAINER_ANIMS(member, tier, container) \
    CREATE_SOFT_POINTER_ANIM(member, DefaultWait, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, BattleWait, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, DefaultIdle1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, DefaultIdle2, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, TurnLeft, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, TurnRight, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Walk, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Run, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, WildBoolStart, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, WildBoolLoop, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, WildBoolEnd, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RestStart, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RestLoop, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RestEnd, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, SleepStart, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, SleepLoop, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, SleepEnd, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Roar, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, WildShot, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Attack1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Attack2, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RangeAttack1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RangeAttack2Start, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RangeAttack2Loop, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, RangeAttack2End, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Damage1, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Damage2, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Glad, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Notice, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, Hate, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, UniqueWaitStart, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, UniqueWaitLoop, tier, container); \
    CREATE_SOFT_POINTER_ANIM(member, UniqueWaitEnd, tier, container)

#define LOAD_ANIM(member, anim, container) member.anim = container.anim.LoadSynchronous();

#define LOAD_ANIMS(member, container) \
    LOAD_ANIM(member, DefaultWait , container); \
    LOAD_ANIM(member, BattleWait , container); \
    LOAD_ANIM(member, DefaultIdle1, container); \
    LOAD_ANIM(member, DefaultIdle2, container); \
    LOAD_ANIM(member, TurnLeft, container); \
    LOAD_ANIM(member, TurnRight, container); \
    LOAD_ANIM(member, Walk, container); \
    LOAD_ANIM(member, Run, container); \
    LOAD_ANIM(member, WildBoolStart, container); \
    LOAD_ANIM(member, WildBoolLoop, container); \
    LOAD_ANIM(member, WildBoolEnd, container); \
    LOAD_ANIM(member, RestStart, container); \
    LOAD_ANIM(member, RestLoop, container); \
    LOAD_ANIM(member, RestEnd, container); \
    LOAD_ANIM(member, SleepStart, container); \
    LOAD_ANIM(member, SleepLoop, container); \
    LOAD_ANIM(member, SleepEnd, container); \
    LOAD_ANIM(member, Roar, container); \
    LOAD_ANIM(member, WildShot, container); \
    LOAD_ANIM(member, Attack1, container); \
    LOAD_ANIM(member, Attack2, container); \
    LOAD_ANIM(member, RangeAttack1, container); \
    LOAD_ANIM(member, RangeAttack2Start, container); \
    LOAD_ANIM(member, RangeAttack2Loop, container); \
    LOAD_ANIM(member, RangeAttack2End, container); \
    LOAD_ANIM(member, Damage1, container); \
    LOAD_ANIM(member, Damage2, container); \
    LOAD_ANIM(member, Glad, container); \
    LOAD_ANIM(member, Notice, container); \
    LOAD_ANIM(member, Hate, container); \
    LOAD_ANIM(member, UniqueWaitStart, container); \
    LOAD_ANIM(member, UniqueWaitLoop, container); \
    LOAD_ANIM(member, UniqueWaitEnd , container); \
    LOAD_ANIM(member, Eye, container)

// Sets default values
APokemon::APokemon() : _entry{0}, _crySound(nullptr), _speed{}, _isRunning{false}, _isSleeping{false}, _pokemonAnimations{}, _pokemonAnimationsSwim{}, _pokemonAnimationsFly{}, _currentMoveTypes{EPokemonMoveType::Walk}, _allowedMoveTypes{static_cast<int32>(EPokemonMoveType::Walk |EPokemonMoveType::Swim)}, _showDebug{false}, _areWalkingAnimationsLoaded{false}, _areSwimmingAnimationsLoaded{false}, _areFlyingAnimationsLoaded{false}
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
            _currentMoveTypes = EPokemonMoveType::Swim;
        }
        else if (CanFly())
        {
            _currentMoveTypes = EPokemonMoveType::Fly;
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
    FString errorText = L"AnimationLoaded";

    _areWalkingAnimationsLoaded = true;

    LOAD_ANIMS(_pokemonAnimations, walkingAnimationsStruct);

    if (_areWalkingAnimationsLoaded && _areSwimmingAnimationsLoaded && _areFlyingAnimationsLoaded)
        OnInitialize();
}

void APokemon::OnSwimmingAnimationsLoaded()
{
    FString errorText = L"AnimationLoaded";

    _areSwimmingAnimationsLoaded = true;

    LOAD_ANIMS(_pokemonAnimationsSwim, swimmingAnimationsStruct);

    if (_areWalkingAnimationsLoaded && _areSwimmingAnimationsLoaded && _areFlyingAnimationsLoaded)
        OnInitialize();
}

void APokemon::OnFlyingAnimationsLoaded()
{
    FString errorText = L"AnimationLoaded";

    _areFlyingAnimationsLoaded = true;

    LOAD_ANIMS(_pokemonAnimationsFly, flyingAnimationsStruct);

    if (_areWalkingAnimationsLoaded && _areSwimmingAnimationsLoaded && _areFlyingAnimationsLoaded)
        OnInitialize();
}

void APokemon::InitializeAnimations(const int32 entry, const FString folderStr)
{
    //auto loadAnimation = [&](const EPokemonAnimations pokemonAnimation, const EPokemonAnimTier pokemonAnimTier)
    //{
    //    FString animationPath = folderStr + UPokemonUtils::GetAnimationNameForPokemon(entry, pokemonAnimation, pokemonAnimTier);
    //    UAnimSequence* anim = LoadObject<UAnimSequence>(nullptr, *animationPath);
    //    if (!anim)
    //    {
    //        animationPath = folderStr + UPokemonUtils::GetAnimationNameForPokemon(entry, pokemonAnimation, pokemonAnimTier);
    //        anim = LoadObject<UAnimSequence>(nullptr, *animationPath);
    //    }

    //    if (!anim && _showDebug && GEngine)
    //    {
    //        FString errorText = L"Error while loading anim for species: " + FString::FromInt(entry) + L" animId: " + FString::FromInt(static_cast<int32>(pokemonAnimation)) + L" tierId: " + FString::FromInt(static_cast<int32>(pokemonAnimTier));
    //        GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, errorText);
    //    }
    //    return anim;
    //};

    //auto animTier = EPokemonAnimTier::Normal;
    //if (CanWalk())
    //{
    //    LOAD_ANIMS(_pokemonAnimations, animTier);
    //}

    //animTier = EPokemonAnimTier::Swimming;
    //if (CanSwim())
    //{
    //    LOAD_ANIMS(_pokemonAnimationsSwim, animTier);
    //}

    //animTier = EPokemonAnimTier::Flying;
    //if (CanFly())
    //{
    //    LOAD_ANIMS(_pokemonAnimationsFly, animTier);
    //}
}

