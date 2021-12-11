// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "EnvironmentQuery/EnvQueryContext.h"
#include "EnvQueryContext_CachedPlayerLoc.generated.h"

/**
 * 
 */
UCLASS()
class GLITTERGOLD_API UEnvQueryContext_CachedPlayerLoc : public UEnvQueryContext
{
	GENERATED_BODY()

protected:
	virtual void ProvideContext(FEnvQueryInstance& QueryInstance, FEnvQueryContextData& ContextData) const override;
	
};
