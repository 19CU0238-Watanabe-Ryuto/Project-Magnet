// ��������I�u�W�F�N�g�������̃N���X
//
// 2021/05/26 �n粗��� �����o�������̊֐����쐬

#include "ItemShootComponent.h"

// Sets default values for this component's properties
UItemShootComponent::UItemShootComponent()
	: m_IsShoot(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UItemShootComponent::BeginPlay()
{
	Super::BeginPlay();	
}


// Called every frame
void UItemShootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}


// �������Ƃ��̊֐�
void UItemShootComponent::SetIsShoot(bool _status)
{
	m_IsShoot = _status;
}