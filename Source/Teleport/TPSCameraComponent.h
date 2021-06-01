// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる（TPSCameraComponentBPからの移行）
// 2021/05/11 渡邊龍音 カメラのロックオンを作成
// 2021/05/12 渡邊龍音 ロックオン対象と一定の距離以内に入った場合、ロックオンを解除する
// 2021/05/14 渡邊龍音 ロックオン可能タグがついたもののみロックオン可能にする
// 2021/05/17 渡邊龍音 カメラの向いている方向のベクトルを取得できるようにする
//					   プレイヤーとオブジェクトの距離がレイの距離以上になったらロックオン解除するように
// 2021/05/24 渡邊龍音 ロックオン可能オブジェクトでも一時的にロックオン不可にする機能を追加
// 2021/05/26 渡邊龍音 自身にロックオン出来ないように
//					   ロックオンの対象とする位置をどのオブジェクトであっても真ん中にする
// 2021/05/27 渡邊龍音 他のコンポーネントからアクセスできるカメラの中央に向かうベクトルを取得できる関数を追加
// 2021/05/29 渡邊龍音 ロックオン対象を広げる
// 2021/05/30 渡邊龍音 ロックオン対象をコライダーによって行うテスト

#pragma once

#include "CoreMinimal.h"
#include "Math/Color.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;
class UBoxComponent;


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

	UFUNCTION()
		void OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	UFUNCTION()
		void OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	// 当たり判定に使うスフィア
	UBoxComponent* m_BoxComponent;

	// レイがほかのActorにあたっているか
	bool m_IsHit;

	// レイがロックオン可能なActorに当たっているか
	bool m_IsHitCanLockOnActor;

	// レイが当たった結果
	FHitResult m_HitResult;

	// レイの衝突判定パラメーター
	FCollisionQueryParams m_CollisionParams;

	// カメラの向いているベクトル
	FVector m_CameraVector;

	// ロックオンしているか
	bool m_IsLockOn;

	// ロックオン時のActor
	AActor* m_LockOnActor;

	// 一時的にロックオン不可にするActor
	AActor* m_CantLockOnActor;

	// カメラコンポーネント
	UCameraComponent* m_CameraComponent;

	// プレイヤーキャラクター
	ACharacter* m_PlayerCharacter;

	// ロックオン可能Actorをすべて入れる
	TArray<AActor*> m_OnCollisionCanLockOnActorArray;

	// 一番近いロックオン可能な距離にあるロックオン可能Actor
	AActor* m_NearCanLockOnActor;

	// private変数読み取り用関数
public:
	// m_IsHit取得用
	UFUNCTION(BlueprintPure)
		bool GetIsHit() { return m_IsHit; }

	//m_IsHitCanLockOnActor取得用
	UFUNCTION(BlueprintPure)
		bool GetIsHitCanLockOnActor() { return m_IsHitCanLockOnActor; }

	// m_HitResult取得用
	UFUNCTION(BlueprintPure)
		FHitResult GetHitResult() { return m_HitResult; }

	// m_CameraVector取得用
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorRaw() { return m_CameraVector; }

	// m_CameraVector取得用（正規化）
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorNormalized()
	{
		FVector normal = m_CameraVector;
		normal.Normalize();
		return normal;
	}

	// m_CameraVector取得用（正規化）
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorOtherActor(FVector _originPos);

	// m_CameraVector取得用（正規化）
	UFUNCTION(BlueprintPure)
		FVector GetCameraVectorNormalizedOtherActor(FVector _originPos);

	// m_IsLockOn取得用
	UFUNCTION(BlueprintPure)
		bool GetIsLockOn() { return m_IsLockOn; }

	// m_LockOnActor取得用
	UFUNCTION(BlueprintPure)
		AActor* GetLockOnActor() { return m_LockOnActor; }

	// m_CameraComponent取得用
	UFUNCTION(BlueprintPure)
		UCameraComponent* GetCameraComponent() { return m_CameraComponent; }

	// m_PlayerCharacter取得用
	UFUNCTION(BlueprintPure)
		ACharacter* GetPlayerCharacter() { return m_PlayerCharacter; }

public:

	// ロックオンを無効化するオブジェクトとの距離
	UPROPERTY(EditAnyWhere)
		float m_DisableLockOnLength;

	// ロックオンできるActorにつけるタグ名
	UPROPERTY(EditAnyWhere)
		FName m_LockOnTag;

	// ボックスサイズ
	UPROPERTY(EditAnyWhere)
		FVector m_BoxCollisionSize;

private:
	// ロックオン処理関数
	void LockOn();

public:
	// 初期化関数
	// 
	// 第一引数...UCameraComponent* 対象になるカメラコンポーネント
	// 第二引数...ACharacter*		対象になるキャラクター
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera, ACharacter* _character, UBoxComponent* _boxConponent);

	// ロックオン状態にする関数
	//
	// 引数なし
	UFUNCTION(BlueprintCallable)
		void SwitchLockOn();

	// ロックオン状態を解除する関数
	//
	// 第一引数...bool	trueでロックオンActorをリセット
	UFUNCTION(BlueprintCallable)
		void DisableLockOn(bool isResetLockOnActor = false);

	// 再度ロックオン状態にする関数
	// 戻り値...再設定できたかどうか
	//
	// 引数なし
	UFUNCTION(BlueprintCallable)
		bool LockOnAgain();

	// ロックオン不可Actorの設定
	// 戻り値...再設定できたかどうか
	//
	// 引数なし
	UFUNCTION(BlueprintCallable)
		void SetCantLockOnActor(AActor* _cantLockOnActor)
	{
		if (_cantLockOnActor != nullptr)
		{
			m_CantLockOnActor = _cantLockOnActor;
			m_OnCollisionCanLockOnActorArray.Remove(m_CantLockOnActor);
		}
		else
		{
			m_OnCollisionCanLockOnActorArray.Add(m_CantLockOnActor);
			m_CantLockOnActor = nullptr;
		}
	}

	UFUNCTION(BlueprintCallable)
		FVector GetLockOnLocation();
};