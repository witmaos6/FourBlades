// Fill out your copyright notice in the Description page of Project Settings.


#include "Animation/FBAnimNotify_AllowNextCombo.h"


UFBAnimNotify_AllowNextCombo::UFBAnimNotify_AllowNextCombo()
{
}

FString UFBAnimNotify_AllowNextCombo::GetNotifyName_Implementation() const
{
	return TEXT("AllowNextCombo");
}

void UFBAnimNotify_AllowNextCombo::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);

	if (MeshComp)
	{
		if (OnAnimNotifyAllowNextCombo.IsBound())
		{
			OnAnimNotifyAllowNextCombo.Execute(); // 딜레이가 필요할 경우가 생길 수 있음, 딜레이가 필요하면 바인딩 된 함수에서 ForNextFrame으로 호출
		}
	}
}
