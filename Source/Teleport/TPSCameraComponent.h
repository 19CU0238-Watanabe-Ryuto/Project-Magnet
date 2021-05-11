// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる
// 2021/05/11 渡邊龍音 カメラのロックオンを作成

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;

UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class TELEPORT_API UTPSCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UTPSCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

private:
	// レイがほかのActorにあたっているか
	bool m_IsHit;

	// レイが当たった結果
	FHitResult m_HitResult;

	// ロックオンしているか
	bool m_IsLockOn;

	// ロックオン時のActor
	AActor* m_LockOnActor;

// private変数読み取り用関数
public:
	// hitResult取得用
	UFUNCTION(BlueprintPure)
		FHitResult GetHitResult() { return m_HitResult; }

	// isHit取得用
	UFUNCTION(BlueprintPure)
		bool GetIsHit() { return m_IsHit; }

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// カメラコンポーネント
	UPROPERTY(EditAnyWhere)
		UCameraComponent* m_CameraComponent;

	// プレイヤーコンポーネント
	UPROPERTY(EditAnyWhere)
		ACharacter* m_PlayerCharacter;

	// ライントレースを有効にするか（レイを飛ばすか）
	UPROPERTY(EditAnyWhere)
		bool m_EnableLineTrace;

	// デバッグ用にレイを描画するか
	UPROPERTY(EditAnyWhere, Category = "Debug")
		bool m_IsDrawDebugLine;

	// デバッグ用レイの表示時間
	UPROPERTY(EditAnyWhere, Category = "Debug")
		float m_DrawDebugLineTime;

	// レイの距離
	UPROPERTY(EditAnyWhere)
		float m_RayLength;

	// レイのオフセット位置
	UPROPERTY(EditAnyWhere)
		FVector m_RayOffset;

private:
	// ロックオン処理関数
	void LockOn();

public:
	// 初期化関数
	// 
	// 第一引数 対象になるカメラコンポーネント
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera, ACharacter* _character);

	// ロックオン状態にする関数
	//
	// 引数なし
	UFUNCTION(BlueprintCallable)
		void SwitchLockOn();
};
