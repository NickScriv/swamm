// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "AnimNotify_DetachItemMesh.generated.h"

/**
 * 
 */
UCLASS()
class GLITTERGOLD_API UAnimNotify_DetachItemMesh : public UAnimNotify
{
	GENERATED_BODY()

		UPROPERTY(EditAnywhere)
		int32 meshToDetach = 0;

protected:
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation) override;
	
};
