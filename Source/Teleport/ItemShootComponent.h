// 反発するオブジェクトを撃つ時のクラス
//
// 2021/05/26 渡邊龍音 撃ち出した時の関数を作成

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ItemShootComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UItemShootComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UItemShootComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// 撃ち出した位置
	FVector m_BeginShootLocation;

	// 現在の威力
	int m_NowDamage;

public:
	UFUNCTION(BlueprintPure)
		int GetNowDamage()
	{
		return m_NowDamage;
	}

public:
	UPROPERTY(BlueprintReadWrite)
	bool m_IsShoot;

	UPROPERTY(BlueprintReadWrite, EditAnyWhere)
	int m_InitialHitDamage;

public:
	// 撃ったときの関数
	//
	// 第一引数：オブジェクトを反発させた位置
	UFUNCTION(BlueprintCallable)
	void Shoot(FVector _shootPos);

	// 距離減衰の計算
	//
	// 第一引数：衝突した位置
	// 第二引数：威力が減衰する距離	（○mごとに△ダメージ減る とした時の ○の部分）
	// 第三引数：減衰する威力		（○mごとに△ダメージ減る とした時の △の部分）
	UFUNCTION(BlueprintCallable)
		int AttenuationDamage(FVector _hitPos, float _attenuationDistance = 100.0f, int _attenuationAmount = 1);

	// 壁にあたった時に撃ち出し位置・威力をを更新する関数
	//
	// 第一引数：衝突した位置
	// 第二引数：壁に衝突したときに掛けられる値
	// 第三引数：威力が減衰する距離	（○mごとに△ダメージ減る とした時の ○の部分）
	// 第四引数：減衰する威力		（○mごとに△ダメージ減る とした時の △の部分）
	UFUNCTION(BlueprintCallable)
		int HitAttenuationDamage(FVector _hitPos, float _attenuationRatio = 0.75f, float _attenuationDistance = 100.0f, int _attenuationAmount = 1);
};
