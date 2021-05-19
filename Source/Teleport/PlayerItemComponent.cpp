// プレイヤーがアイテムを取得するためのクラス
//
// 2021/05/19 渡邊龍音 スコアを管理できる

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
