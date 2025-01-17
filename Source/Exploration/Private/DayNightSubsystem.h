// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "DayNightSubsystem.generated.h"

UENUM(BlueprintType)
enum class EDayTime : uint8
{
	Day = 0U,
	Night = 1U
};

UCLASS()
class UDayNightSubsystem : public UTickableWorldSubsystem
{
	GENERATED_BODY()
	
public:
	
	TStatId GetStatId() const override
	{
		return GetStatID();
	}

	void Tick(float deltaTime) override;

	UFUNCTION(BlueprintCallable)
	EDayTime GetCurrentTimeDay() const;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _dayLength = 10.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _dayTimer;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	float _dayDonePercentage;
};
