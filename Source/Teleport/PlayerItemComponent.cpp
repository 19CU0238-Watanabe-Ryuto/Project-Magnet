// �v���C���[���A�C�e�����擾���邽�߂̃N���X
//
// 2021/05/19 �n粗��� �X�R�A���Ǘ��ł���

#include "PlayerItemComponent.h"

// Sets default values for this component's properties
UPlayerItemComponent::UPlayerItemComponent()
	: m_Score(0)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UPlayerItemComponent::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UPlayerItemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}
