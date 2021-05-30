// 反発するオブジェクトを撃つ時のクラス
//
// 2021/05/26 渡邊龍音 撃ち出した時の関数を作成
// 2021/05/30 渡邊龍音 誰が撃ち出したのか分かるように

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


// 撃ったときの関数
void UItemShootComponent::Shoot(FVector _shootPos, AActor* _shootPlayer)
{
	m_IsShoot = true;
	m_BeginShootLocation = _shootPos;
	m_WhoShoot = _shootPlayer;
	m_NowDamage = m_InitialHitDamage;
}


// 距離減衰の計算
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

// 壁にあたった時に撃ち出し位置・威力をを更新する関数
int UItemShootComponent::HitAttenuationDamage(FVector _hitPos, float _attenuationRatio/* = 0.75f */, float _attenuationDistance/* = 100.0f */, int _attenuationAmount/* = 1 */)
{
	AttenuationDamage(_hitPos, _attenuationDistance, _attenuationAmount);
	m_BeginShootLocation = _hitPos;

	m_NowDamage *= _attenuationRatio;

	return m_NowDamage;
}