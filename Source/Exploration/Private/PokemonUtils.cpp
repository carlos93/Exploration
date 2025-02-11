// Fill out your copyright notice in the Description page of Project Settings.PokemonAnimations

#include "PokemonUtils.h"
#include "GameFramework/Character.h"
#include "AnimNode_SequencePlayer.generated.h"
#include <string>
#include <type_traits>
#include "Pokemon.h"
#include <format>

TMap<EPokemonAnimations, FString> UPokemonUtils::AnimationPathNames;
bool UPokemonUtils::isDatabaseLoaded;

void UPokemonUtils::InitDatabase()
{
    if (isDatabaseLoaded)
        return;

    AnimationPathNames.Add(EPokemonAnimations::DefaultWait, "defaultwait01_loop");
    AnimationPathNames.Add(EPokemonAnimations::BattleWait, "battlewait01_loop");
    AnimationPathNames.Add(EPokemonAnimations::DefaultIdle1, "defaultidle01");
    AnimationPathNames.Add(EPokemonAnimations::DefaultIdle2, "defaultidle02");
    AnimationPathNames.Add(EPokemonAnimations::TurnRight, "turn_r090");
    AnimationPathNames.Add(EPokemonAnimations::TurnLeft, "turn_l090");
    AnimationPathNames.Add(EPokemonAnimations::Walk, "walk01_loop");
    AnimationPathNames.Add(EPokemonAnimations::Run, "run01_loop");
    AnimationPathNames.Add(EPokemonAnimations::WildBoolStart, "wildbool02_start");
    AnimationPathNames.Add(EPokemonAnimations::WildBoolLoop, "wildbool02_loop");
    AnimationPathNames.Add(EPokemonAnimations::WildBoolEnd, "wildbool02_end");
    AnimationPathNames.Add(EPokemonAnimations::RestStart, "rest01_start");
    AnimationPathNames.Add(EPokemonAnimations::RestLoop, "rest01_loop");
    AnimationPathNames.Add(EPokemonAnimations::RestEnd, "rest01_end");
    AnimationPathNames.Add(EPokemonAnimations::SleepStart, "sleep01_start");
    AnimationPathNames.Add(EPokemonAnimations::SleepLoop, "sleep01_loop");
    AnimationPathNames.Add(EPokemonAnimations::SleepEnd, "sleep01_end");
    AnimationPathNames.Add(EPokemonAnimations::Roar, "roar01");
    AnimationPathNames.Add(EPokemonAnimations::WildShot, "wildshot02");
    AnimationPathNames.Add(EPokemonAnimations::Attack1, "attack01");
    AnimationPathNames.Add(EPokemonAnimations::Attack2, "attack02");
    AnimationPathNames.Add(EPokemonAnimations::RangeAttack1, "rangeattack01");
    AnimationPathNames.Add(EPokemonAnimations::RangeAttack2Start, "rangeattack02_start");
    AnimationPathNames.Add(EPokemonAnimations::RangeAttack2Loop, "rangeattack02_loop");
    AnimationPathNames.Add(EPokemonAnimations::RangeAttack2End, "rangeattack02_end");
    AnimationPathNames.Add(EPokemonAnimations::Damage1, "damage01");
    AnimationPathNames.Add(EPokemonAnimations::Damage2, "damage02");
    AnimationPathNames.Add(EPokemonAnimations::Glad, "glad01");
    AnimationPathNames.Add(EPokemonAnimations::Notice, "notice01");
    AnimationPathNames.Add(EPokemonAnimations::Hate, "hate01");
    AnimationPathNames.Add(EPokemonAnimations::UniqueWaitStart, "uniquewait01_start");
    AnimationPathNames.Add(EPokemonAnimations::UniqueWaitLoop, "uniquewait01_loop");
    AnimationPathNames.Add(EPokemonAnimations::UniqueWaitEnd, "uniquewait01_end");

    isDatabaseLoaded = true;
}

FString UPokemonUtils::GetAnimationNameForPokemon(const int32 entry, const EPokemonAnimations pokemonAnimation, const EPokemonAnimTier pokemonAnimTier, const uint8 gender)
{
    std::string entryStr = std::format("{:0>4}", entry);
    std::string animTier = std::format("{:0>2}", static_cast<uint8>(pokemonAnimTier));
    std::string animId = std::format("{:0>3}", static_cast<int32>(pokemonAnimation));
    std::string formStr = std::format("{:0>2}", gender); // Form ID (alternative forms like Giratina)
    std::string colorsStr = std::format("{:0>2}", "0"); // Color ID (only color change, like arceus)

    FString animFStr = TEXT("");
    if (AnimationPathNames.Contains(pokemonAnimation))
    {
        animFStr = AnimationPathNames[pokemonAnimation];
    }
    std::string animStr = std::string(TCHAR_TO_UTF8(*animFStr));
    std::string path = std::format("pm{}_{}_{}_{}{}_{}", entryStr, formStr, colorsStr, animTier, animId, animStr);
    std::string path2 = path + "." + path;
    return FString(path2.c_str());
}

FString UPokemonUtils::FirstLetterToUppercase(FString name)
{
    if (name.Len() > 0)
    {
        auto firstLetter = name.LeftChop(name.Len() - 1U);
        firstLetter = firstLetter.ToUpper();
        name = firstLetter.Append(name.RightChop(1));
    }

    return name;
}

