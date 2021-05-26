// 反発するオブジェクトを撃つ時のクラス
//
// 2021/05/26 渡邊龍音 撃ち出した時の関数を作成

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


// 撃ったときの関数
void UItemShootComponent::SetIsShoot(bool _status)
{
	m_IsShoot = _status;
}