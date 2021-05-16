// 磁力の能力を管理するクラス
//
// 2021/05/12 - 5/13 渡邊龍音 自身の引き寄せ・反発を行う（MagnetComponentBPからの移行）

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagnetComponent.generated.h"

class UTPSCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TELEPORT_API UMagnetComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UMagnetComponent();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// TPSCameraComponent
	UTPSCameraComponent* m_TPSCamera;

	// 引き寄せ状態であるか
	bool m_IsAttract;

	// 停止状態であるか
	bool m_IsFreeze;

	// プレイヤーを引き寄せる状態か
	bool m_IsAttractPlayer;

	// プレイヤーのもともとのgravityScale
	float m_playerOriginGravityScale;

	// 引き寄せ移動する時の対象Actorの位置
	FVector m_TargetActorLocation;

	// private変数取得用
public:
	// m_IsFreeze取得用
	UFUNCTION(BlueprintPure)
		bool GetIsFreeze() { return m_IsFreeze; }

public:
	// 引き寄せる力
	UPROPERTY(EditAnyWhere)
		float m_AttractPower;

	// プレイヤーを引き寄せる時のオブジェクトのタグ名
	UPROPERTY(EditAnyWhere)
		FName m_AttractPlayerTagName;

	// オブジェクトを引き寄せる時のオブジェクトのタグ名
	UPROPERTY(EditAnyWhere)
		FName m_AttractObjectTagName;

private:
	// 引き寄せ関数
	//
	// 
	void Attract(float _DeltaTime);

public:
	// 初期化関数
	//
	// 第一引数...キャラクターについているTPSCameraComponent
	UFUNCTION(BlueprintCallable)
		void Init(UTPSCameraComponent* _TPSCameraComponent);

	// 引き寄せ状態切り替え関数
	//
	//
	UFUNCTION(BlueprintCallable)
		void SwitchAttract();

	// 当たった時に呼ぶ関数
	//
	//
	UFUNCTION(BlueprintCallable)
		void Hit(AActor* _hitActor);
};
