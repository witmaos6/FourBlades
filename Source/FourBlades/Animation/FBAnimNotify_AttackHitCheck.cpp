// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FBAnimNotify_AttackHitCheck.h"

UFBAnimNotify_AttackHitCheck::UFBAnimNotify_AttackHitCheck()
{

}

FString UFBAnimNotify_AttackHitCheck::GetNotifyName_Implementation() const
{
	return TEXT("AttackHitCheck");
}

void UFBAnimNotify_AttackHitCheck::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (OnAnimNotifyAttackHitCheck.IsBound())
		{
			OnAnimNotifyAttackHitCheck.Execute(); // 딜레이가 필요할 경우가 생길 수 있음, 딜레이가 필요하면 바인딩 된 함수에서 ForNextFrame으로 호출
		}
	}
}
