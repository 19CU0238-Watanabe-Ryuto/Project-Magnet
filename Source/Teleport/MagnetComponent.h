// 磁力の能力を管理するクラス
//
// 2021/05/12 - 5/13 渡邊龍音 MagnetComponentBPからの移行
// 2021/05/14		 渡邊龍音 オブジェクトの引き寄せを行う
// 2021/05/16 - 5/17 渡邊龍音 自身・オブジェクトの反発を行う
// 2021/05/17		 渡邊龍音 引き寄せの処理をVInterpからAddForceに変更
//							  能力使用時にロックオンを解除するように
// 2020/05/21		 渡邊龍音 能力の移動をVInterpに戻す
// 2020/05/24		 渡邊龍音 引き寄せのデフォ値を変更
//							  引き寄せ・反発のプロパティをプレイヤー用とオブジェクト用に分ける
//							  オブジェクト引き寄せ状態でプレイヤーの引き寄せを可能にする
//							  プレイヤーを引き寄せるときに足元の位置を参照
//							  移動のやりやすさを向上

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

	// 能力で移動するActor
	AActor* m_SmallerPlayerActor;

	// 能力で移動しないActor
	AActor* m_GreaterPlayerActor;

	// プレイヤーの足元の位置のSceneComponent
	USceneComponent* m_PlayerAnchor;

	// ロックオンしたActorのStaticMesh
	UStaticMeshComponent* m_LockOnActorStaticMesh;

	// 能力で移動するActorのStaticMesh
	UStaticMeshComponent* m_SmallerActorStaticMesh;

	// 磁力能力で引き寄せを行ったときに固定するオブジェクトの位置
	UStaticMeshComponent* m_AttractFloatingPoint;

	// 引き寄せ状態であるか
	bool m_IsAttract;

	// 反発状態であるか
	bool m_IsRepulsion;

	// 停止状態であるか
	bool m_IsFreeze;

	// オブジェクトを追従させている状態か
	bool m_IsAttractObject;

	// 反発能力の対象がプレイヤーか（能力でプレイヤーが動くかオブジェクトが動くか）
	bool m_IsRepulsionOfAbilityPlayer;

	// プレイヤーのもともとのgravityScale
	float m_playerOriginGravityScale;

	// 引き寄せ移動する時の対象Actorの位置
	//FVector m_TargetActorLocation;

	// private変数取得用
public:
	// m_IsFreeze取得用
	UFUNCTION(BlueprintPure)
		bool GetIsFreeze() { return m_IsFreeze; }

	// m_IsAttract取得用
	UFUNCTION(BlueprintPure)
		bool GetIsAttract() { return m_IsAttract; }

public:
	// プレイヤーを引き寄せる力
	UPROPERTY(EditAnyWhere)
		float m_AttractPlayerPower;

	// オブジェクトを引き寄せる力
	UPROPERTY(EditAnyWhere)
		float m_AttractObjectPower;

	// オブジェクトに対してプレイヤーが反発する力
	UPROPERTY(EditAnyWhere)
		float m_RepulsionPlayerPower;

	// プレイヤーがオブジェクトを反発させる力
	UPROPERTY(EditAnyWhere)
		float m_RepulsionObjectPower;

	//  能力の対象がプレイヤーの時のオブジェクトのタグ名
	UPROPERTY(EditAnyWhere)
		FName m_TargetOfAbilityPlayerTagName;

	// 能力の対象がオブジェクトの時のオブジェクトのタグ名
	UPROPERTY(EditAnyWhere)
		FName m_TargetOfAbilityObjectTagName;

private:
	// 引き寄せ処理関数
	//
	// 
	void Attract(float _DeltaTime);

	// 反発処理関数
	//
	// 
	void Repulsion(float _DeltaTime);

public:
	// 初期化関数
	//
	// 第一引数...キャラクターについているTPSCameraComponent
	UFUNCTION(BlueprintCallable)
		void Init(UTPSCameraComponent* _TPSCameraComponent, UStaticMeshComponent* _attractFloatingPoint, USceneComponent* _playerFoot);

	// 引き寄せ状態切り替え関数
	//
	//
	UFUNCTION(BlueprintCallable)
		void SwitchAttract();

	// 反発状態切り替え関数
	//
	//
	UFUNCTION(BlueprintCallable)
		void SwitchRepulsion();

	// 当たった時に呼ぶ関数
	//
	//
	UFUNCTION(BlueprintCallable)
		void Hit(AActor* _hitActor);
};
