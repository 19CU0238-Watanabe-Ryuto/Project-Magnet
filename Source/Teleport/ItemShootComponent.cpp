// 反発するオブジェクトを撃つ時のクラス
//
// 2021/05/26 渡邊龍音 撃ち出した時の関数を作成
// 2021/05/30 渡邊龍音 誰が撃ち出したのか分かるように
// 2021/06/01 渡邊龍音 誰が撃ち出したのか ->誰かが持っているときに誰が持っているのか分かるように

#include "ItemShootComponent.h"

// Sets default values for this component's properties
UItemShootComponent::UItemShootComponent()
	: m_IsShoot(false)
	, m_WhoHave(nullptr)
	, m_BeginShootLocation(FVector::ZeroVector)
	, m_NowDamage(0)
	, m_InitialHitDamage(200)
	, actorResetFlg(false)
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

	if (actorResetFlg && !m_IsShoot)
	{
		actorResetFlg = false;
		m_WhoHave = nullptr;
	}
}


// 取ったときの関数
UFUNCTION(BlueprintCallable)
void UItemShootComponent::GetItem(AActor* _getActor)
{
	m_WhoHave = _getActor;

	actorResetFlg = false;
}


// 撃ったときの関数
void UItemShootComponent::Shoot(FVector _shootPos)
{
	m_IsShoot = true;
	m_BeginShootLocation = _shootPos;
	m_NowDamage = m_InitialHitDamage;

	actorResetFlg = true;
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