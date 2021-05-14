// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる（TPSCameraComponentBPからの移行）
// 2021/05/11 渡邊龍音 カメラのロックオンを作成
// 2021/05/12 渡邊龍音 ロックオン対象と一定の距離以内に入った場合、ロックオンを解除する

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TELEPORT_API UTPSCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTPSCameraComponent();

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:
	// レイがほかのActorにあたっているか
	bool m_IsHit;

	// レイが当たった結果
	FHitResult m_HitResult;

	// ロックオンしているか
	bool m_IsLockOn;

	// ロックオン時のActor
	AActor* m_LockOnActor;

	// カメラコンポーネント
	UCameraComponent* m_CameraComponent;

	// プレイヤーキャラクター
	ACharacter* m_PlayerCharacter;

// private変数読み取り用関数
public:
	// m_HitResult取得用
	UFUNCTION(BlueprintPure)
		FHitResult GetHitResult() { return m_HitResult; }

	// m_IsHit取得用
	UFUNCTION(BlueprintPure)
		bool GetIsHit() { return m_IsHit; } 

	// m_IsLockOn取得用
	UFUNCTION(BlueprintPure)
		bool GetIsLockOn() { return m_IsLockOn; }

	// m_LockOnActor取得用
	UFUNCTION(BlueprintPure)
		AActor* GetIsLockOnActor() { return m_LockOnActor; }

	// m_CameraComponent取得用
	UFUNCTION(BlueprintPure)
		UCameraComponent* GetCameraComponent() { return m_CameraComponent; }

	// m_PlayerCharacter取得用
	UFUNCTION(BlueprintPure)
		ACharacter* GetPlayerCharacter() { return m_PlayerCharacter; }

public:

	// ライントレースを有効にするか（レイを飛ばすか）
	UPROPERTY(EditAnyWhere)
		bool m_EnableLineTrace;

	// デバッグ用にレイを描画するか
	UPROPERTY(EditAnyWhere, Category = "Debug")
		bool m_IsDrawDebugLine;

	// デバッグ用レイの表示時間
	UPROPERTY(EditAnyWhere, Category = "Debug")
		float m_DrawDebugLineTime;

	// デバッグ用レイのロックオン時の色
	UPROPERTY(EditAnyWhere, Category = "Debug")
		FColor m_LockOnRayColor;

	// デバッグ用レイの非ロックオン時の色
	UPROPERTY(EditAnyWhere, Category = "Debug")
		FColor m_NoLockOnRayColor;

	// デバッグ用レイの他Actorとのヒット時の色
	UPROPERTY(EditAnyWhere, Category = "Debug")
		FColor m_HitRayColor;

	// レイの距離
	UPROPERTY(EditAnyWhere)
		float m_RayLength;

	// レイのオフセット位置
	UPROPERTY(EditAnyWhere)
		FVector m_RayOffset;

	// ロックオンを無効化するオブジェクトとの距離
	UPROPERTY(EditAnyWhere)
		float m_DisableLockOnLength;

private:
	// ロックオン処理関数
	void LockOn();

public:
	// 初期化関数
	// 
	// 第一引数...対象になるカメラコンポーネント
	// 第二引数...対象になるキャラクター
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera, ACharacter* _character);

	// ロックオン状態にする関数
	//
	// 引数なし
	UFUNCTION(BlueprintCallable)
		void SwitchLockOn();
};
