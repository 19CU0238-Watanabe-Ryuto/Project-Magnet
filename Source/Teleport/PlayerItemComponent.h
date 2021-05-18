// プレイヤーがアイテムを取得するためのクラス
//
// 2021/05/18 渡邊龍音 カメラの向いている方向のベクトルを取得できるようにする

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "PlayerItemComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UPlayerItemComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UPlayerItemComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// スコア変数
	int m_Score;

public:
	// スコア追加関数
	//
	//
	UFUNCTION(BlueprintCallable)
		void AddScore(int _score)
	{
		m_Score += _score;
	}

	// スコア取得関数
	//
	//
	UFUNCTION(BlueprintPure)
		int GetScore()
	{
		return m_Score;
	}
};
