// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる（TPSCameraComponentBPからの移行）
// 2021/05/11 渡邊龍音 カメラのロックオンを作成
// 2021/05/12 渡邊龍音 ロックオン対象と一定の距離以内に入った場合、ロックオンを解除する
// 2021/05/14 渡邊龍音 ロックオン可能タグがついたもののみロックオン可能にする
// 2021/05/17 渡邊龍音 カメラの向いている方向のベクトルを取得できるようにする

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: m_IsHit(false)
	, m_IsHitCanLockOnActor(false)
	, m_IsLockOn(false)
	, m_LockOnActor(nullptr)
	, m_CameraComponent(nullptr)
	, m_PlayerCharacter(nullptr)
	, m_EnableLineTrace(true)
	, m_IsDrawDebugLine(true)
	, m_DrawDebugLineTime(0.5f)
	, m_LockOnRayColor(FColor::Red)
	, m_NoLockOnRayColor(FColor::Green)
	, m_HitRayColor(FColor::Blue)
	, m_RayLength(5000.0f)
	, m_RayOffset(FVector::ZeroVector)
	, m_DisableLockOnLength(100.0f)
	, m_LockOnTag("CanLockOn")
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UTPSCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ライントレースをしない場合、関数を終了
	if (!m_EnableLineTrace)
	{
		return;
	}

	// カメラコンポーネント未設定の場合、ログを表示し関数を終了
	if (m_CameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Camera is null."));
		return;
	}

	// プレイヤーコンポーネント未設定の場合、ログを表示し関数を終了
	if (m_PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] PlayerCharacter is null."));
		return;
	}

	// レイ開始位置
	FVector start = m_CameraComponent->GetComponentLocation();

	// レイ終了位置
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_RayLength;
	FVector end = start + forwardVec + m_RayOffset;

	// カメラの方向ベクトル格納
	m_CameraVector = end - start;

	// レイの当たっているものに応じてレイの色を変更
	FColor lineColor;

	if (m_IsLockOn)
	{
		lineColor = m_LockOnRayColor;
		m_IsHitCanLockOnActor = true;
	}
	else if (m_HitResult.GetActor() == nullptr)
	{
		lineColor = m_NoLockOnRayColor;
		m_IsHitCanLockOnActor = false;
	}
	else if (m_HitResult.GetActor()->ActorHasTag(m_LockOnTag))
	{
		lineColor = m_HitRayColor;
		m_IsHitCanLockOnActor = true;
	}
	else
	{
		lineColor = m_NoLockOnRayColor;
		m_IsHitCanLockOnActor = false;
	}

	// デバッグ確認用のラインを描画
	if (m_IsDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), start, end, lineColor, false, m_DrawDebugLineTime);
	}

	// レイを飛ばし当たり判定を取る
	m_IsHit = GetWorld()->LineTraceSingleByChannel(m_HitResult, start, end, ECollisionChannel::ECC_Visibility);

	// ロックオン処理
	LockOn();
}


// ロックオン処理関数
void UTPSCameraComponent::LockOn()
{
	// ロックオン状態でなければ処理を終了
	if (!m_IsLockOn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not lock-On."));
		return;
	}

	// ロックオン対象が有効でなければ処理を終了
	if (m_LockOnActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] LockOnActor is nullptr."));
		return;
	}

	FVector playerPos(m_PlayerCharacter->GetActorLocation().X, m_PlayerCharacter->GetActorLocation().Y, 0.0f);
	FVector targetPos(m_LockOnActor->GetActorLocation().X, m_LockOnActor->GetActorLocation().Y, 0.0f);
	float targetDistance = (playerPos - targetPos).Size();

	// ロックオン対象との距離が設定値以下であればロックオン状態を解除し処理を終了
	if (targetDistance < m_DisableLockOnLength)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Player got within a certain distance, so player disabled lock-on."));
		//m_IsLockOn = false;
		return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Lock-on Actor is \"%s\"."), *m_LockOnActor->GetName());
	FVector targetLocation = m_LockOnActor->GetActorLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), targetLocation);

	m_PlayerCharacter->GetController()->SetControlRotation(rot);
}


// 初期化関数
void UTPSCameraComponent::Init(UCameraComponent* _camera, ACharacter* _character)
{
	m_CameraComponent = _camera;
	m_PlayerCharacter = _character;
}


// ロックオン状態にする関数
void UTPSCameraComponent::SwitchLockOn()
{
	// ロックオン状態のスイッチ
	m_IsLockOn = !m_IsLockOn;

	if (m_IsLockOn)
	{
		// レイの当たっているActorを保持
		m_LockOnActor = m_HitResult.GetActor();

		// Actorを取得できていない
		if (m_LockOnActor == nullptr)
		{
			m_IsLockOn = false;
		}
		// ロックオン出来ないActorであれば処理を終了
		else if (!m_LockOnActor->ActorHasTag(m_LockOnTag))
		{
			UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Lock-on enable tag is not set for the Actor who tried to lock-on."));
			m_IsLockOn = false;
		}
	}
	else
	{
		m_LockOnActor = nullptr;
	}
}
