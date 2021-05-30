// ��������I�u�W�F�N�g�������̃N���X
//
// 2021/05/26 �n粗��� �����o�������̊֐����쐬
// 2021/05/30 �n粗��� �N�������o�����̂�������悤��

#include "ItemShootComponent.h"

// Sets default values for this component's properties
UItemShootComponent::UItemShootComponent()
	: m_IsShoot(false)
	, m_WhoShoot(nullptr)
	, m_BeginShootLocation(FVector::ZeroVector)
	, m_NowDamage(0)
	, m_InitialHitDamage(200)
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
void UItemShootComponent::Shoot(FVector _shootPos, AActor* _shootPlayer)
{
	m_IsShoot = true;
	m_BeginShootLocation = _shootPos;
	m_WhoShoot = _shootPlayer;
	m_NowDamage = m_InitialHitDamage;
}


// ���������̌v�Z
int UItemShootComponent::AttenuationDamage(FVector _hitPos, float _attenuationDistance/* = 100.0f */, int _attenuationAmount/* = 1 */)
{
	float length = (_hitPos - m_BeginShootLocation).Size();

	if (_attenuationDistance == 0.0f)
	{
		_attenuationDistance = length;
	}

	m_NowDamage -= (length / _attenuationDistance * _attenuationAmount);

	return m_NowDamage;
}

// �ǂɂ����������Ɍ����o���ʒu�E�З͂����X�V����֐�
int UItemShootComponent::HitAttenuationDamage(FVector _hitPos, float _attenuationRatio/* = 0.75f */, float _attenuationDistance/* = 100.0f */, int _attenuationAmount/* = 1 */)
{
	AttenuationDamage(_hitPos, _attenuationDistance, _attenuationAmount);
	m_BeginShootLocation = _hitPos;

	m_NowDamage *= _attenuationRatio;

	return m_NowDamage;
}