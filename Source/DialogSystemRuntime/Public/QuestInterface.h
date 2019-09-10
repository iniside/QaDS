// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "QuestInterface.generated.h"

// This class does not need to be modified.
UINTERFACE(MinimalAPI)
class UQuestInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class DIALOGSYSTEMRUNTIME_API IQuestInterface
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	class UQuestComponent* GetQuestComponent() const = 0;
};
