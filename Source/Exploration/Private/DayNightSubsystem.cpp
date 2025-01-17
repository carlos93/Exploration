// Fill out your copyright notice in the Description page of Project Settings.


#include "DayNightSubsystem.h"
#include <math.h>

EDayTime UDayNightSubsystem::GetCurrentTimeDay() const
{
    if (_dayDonePercentage > 0.2f && _dayDonePercentage < 0.8f)
    {
        return EDayTime::Day;
    }
    else
    {
        return EDayTime::Night;
    }
}

void UDayNightSubsystem::Tick(float deltaTime)
{
    _dayTimer += deltaTime;

    if (_dayTimer > _dayLength)
    {
        _dayTimer -= _dayLength; 
    }

    _dayDonePercentage = _dayTimer / _dayLength;
    _dayDonePercentage = std::clamp(_dayDonePercentage, 0.0f, 1.0f);
}