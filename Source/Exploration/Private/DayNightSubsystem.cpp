// Fill out your copyright notice in the Description page of Project Settings.


#include "DayNightSubsystem.h"
#include <math.h>

EDayTime UDayNightSubsystem::GetCurrentTimeDay() const
{
    EDayTime dayTime = EDayTime::Day;

    int32 hour = FMath::TruncToInt(_dayDonePercentage * 24.0f);
    if (hour < 6)
    {
        dayTime = EDayTime::Night;
    }
    else if (hour < 9)
    {
        dayTime = EDayTime::Dawn;
    }
    else if (hour < 18)
    {
        dayTime = EDayTime::Day;
    }
    else if (hour < 21)
    {
        dayTime = EDayTime::Dusk;
    }
    else
    {
        dayTime = EDayTime::Night;
    }

    return dayTime;
}

void UDayNightSubsystem::Tick(float deltaTime)
{
    if (_isPaused)
    {
        return;
    }

    _dayTimer += deltaTime;

    if (_dayTimer > _dayLength)
    {
        _dayTimer -= _dayLength; 
    }

    _dayDonePercentage = _dayTimer / _dayLength;
    _dayDonePercentage = std::clamp(_dayDonePercentage, 0.0f, 1.0f);
}
