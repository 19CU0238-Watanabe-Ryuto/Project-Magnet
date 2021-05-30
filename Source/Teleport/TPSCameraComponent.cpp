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

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: m_IsHit(false)
	, m_IsHitCanLockOnActor(false)
	, m_CollisionParams(FCollisionQueryParams::DefaultQueryParam)
	, m_IsLockOn(false)
	, m_LockOnActor(nullptr)
	, m_CantLockOnActor(nullptr)
	, m_CameraComponent(nullptr)
	, m_PlayerCharacter(nullptr)
	, m_NearCanLockOnActor(nullptr)
	, m_EnableLineTrace(true)
	, m_IsDrawDebugLine(true)
	, m_DrawDebugLineTime(0.5f)
	, m_LockOnRayColor(FColor::Red)
	, m_NoLockOnRayColor(FColor::Green)
	, m_HitRayColor(FColor::Blue)
	, m_CanLockOnLength(15000.0f)
	, m_CanLockOnDegree(70.0f)
	, m_RayOffset(FVector::ZeroVector)
	, m_DisableLockOnLength(0.0f)
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
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_CanLockOnLength;
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
	else if (m_NearCanLockOnActor != nullptr)
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
	m_IsHit = GetWorld()->LineTraceSingleByChannel(m_HitResult, start, end, ECollisionChannel::ECC_Pawn, m_CollisionParams);

	// 一番近いActorがどれか検索
	float nearLength = m_CanLockOnLength;

	// カメラからのレイが当たった場所を保存
	FVector hitPos = FVector::ZeroVector;
	if (m_IsHit)
	{
		hitPos = m_HitResult.Location;
	}

	// 一番近いActorをリセットする
	m_NearCanLockOnActor = nullptr;

	// 保存しているActorの個数分ループする
	for (AActor* lockOnActor : m_CanLockOnActorArray)
	{
		if (!lockOnActor)
		{
			continue;
		}
		// Actorのバウンディングボックスを取得
		FVector bound;
		FVector ex;
		lockOnActor->GetActorBounds(true, bound, ex);

		// Actorとの距離を計測
		float length = (bound - m_PlayerCharacter->GetActorLocation()).Size();

		// Actorとの角度を算出
		FVector cameraVec = m_CameraComponent->GetForwardVector();
		FRotator objectRot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), bound);
		FVector objectVec = UKismetMathLibrary::GetForwardVector(objectRot);

		float distance = FVector::Dist(cameraVec, objectVec);
		float deg = UKismetMathLibrary::DegAcos(distance);

		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Degree = %f Actor = %s"), deg, *lockOnActor->GetName());

		if (deg > m_CanLockOnDegree)
		{
			// カメラからのレイがなにも当たっていなければ
			if (!m_IsHit)
			{
				// ロックオン可能範囲でかつ、カメラの角度が範囲内であれば
				if (length < nearLength)
				{
					m_NearCanLockOnActor = lockOnActor;
					nearLength = length;
				}
			}
			else
			{
				// カメラのレイの当たった位置からActorまでレイを飛ばす
				FHitResult res;

				// レイの衝突をお互いに当たらないようにする
				FCollisionQueryParams colParams;
				colParams.AddIgnoredActor(m_PlayerCharacter);
				colParams.AddIgnoredActor(lockOnActor);

				bool isHit = GetWorld()->LineTraceSingleByChannel(res, bound, hitPos, ECollisionChannel::ECC_Pawn, colParams);

				if (isHit && res.Actor != nullptr)
				{
					UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Name = %s"), *res.Actor->GetName());
				}

				// ロックオン可能範囲でかつ、カメラの角度が範囲内で、さらにレイが当たっていなければ（遮るものがない）
				if (length < nearLength && !isHit)
				{
					m_NearCanLockOnActor = lockOnActor;
					nearLength = length;
				}
			}
		}
	}

	if (m_NearCanLockOnActor != nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Near Actor = %s (Length = %f)"), *m_NearCanLockOnActor->GetName(), nearLength);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Near Actor is None"));
	}
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

	// バウンディングボックス取得
	FVector targetOrigin;
	FVector targetExtent;

	m_LockOnActor->GetActorBounds(true, targetOrigin, targetExtent);

	FVector playerPos(m_PlayerCharacter->GetActorLocation().X, m_PlayerCharacter->GetActorLocation().Y, 0.0f);
	FVector targetPos(m_LockOnActor->GetActorLocation().X, m_LockOnActor->GetActorLocation().Y, 0.0f);
	float targetDistance = (playerPos - targetPos).Size();

	// ロックオンしているオブジェクトとの距離がレイの距離以上になったらロックオン状態解除
	if (targetDistance >= m_CanLockOnLength)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Player disabled lock-on because the distance between the player and the object is longer than RayLength."));
		m_IsLockOn = false;
		return;
	}

	// ロックオン対象との距離が設定値以下であればロックオン状態を解除し処理を終了
	if (targetDistance < m_DisableLockOnLength)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Player got within a certain distance, so player disabled lock-on."));
		//m_IsLockOn = false;
		return;
	}

	UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Lock-on Actor is \"%s\"."), *m_LockOnActor->GetName());

	//FVector targetLocation = m_LockOnActor->GetActorLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), targetOrigin);

	m_PlayerCharacter->GetController()->SetControlRotation(rot);
}


// m_CameraVector取得用（正規化）
FVector UTPSCameraComponent::GetCameraVectorOtherActor(FVector _originPos)
{
	if (m_CameraComponent != nullptr)
	{
		// レイ開始位置
		FVector start = m_CameraComponent->GetComponentLocation();

		// レイ終了位置
		FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_CanLockOnLength;
		FVector end = start + forwardVec + m_RayOffset;

		FVector vector = end - _originPos;

		return vector;
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] CameraComponent is nullptr."))
			return FVector::ZeroVector;
	}
}


// m_CameraVector取得用（正規化）
FVector UTPSCameraComponent::GetCameraVectorNormalizedOtherActor(FVector _originPos)
{
	FVector normal = GetCameraVectorOtherActor(_originPos);
	normal.Normalize();

	return normal;
}


// 初期化関数
void UTPSCameraComponent::Init(UCameraComponent* _camera, ACharacter* _character)
{
	m_CameraComponent = _camera;
	m_PlayerCharacter = _character;

	// レイの衝突を自分自身のキャラクターに当たらないようにする
	m_CollisionParams.AddIgnoredActor(_character);

	m_CanLockOnActorArray.Empty();
	// ロックオン可能Actorをすべて配列に入れる
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), m_LockOnTag, m_CanLockOnActorArray);

	// 自身を削除
	m_CanLockOnActorArray.Remove(_character);

	// レイの衝突をロックオン可能なActorには無効にする
	m_CollisionParams.AddIgnoredActors(m_CanLockOnActorArray);
}


// ロックオン状態にする関数
void UTPSCameraComponent::SwitchLockOn()
{
	// ロックオン状態のスイッチ
	m_IsLockOn = !m_IsLockOn;

	if (m_IsLockOn)
	{
		/*
		// レイの当たっているActorを保持
		m_LockOnActor = m_HitResult.GetActor();
		*/

		// 一番近くのActorを保持
		m_LockOnActor = m_NearCanLockOnActor;

		// Actorを取得できていない
		if (m_LockOnActor == nullptr)
		{
			m_IsLockOn = false;
		}
		// ロックオン出来ないActorであれば処理を終了
		/*
		else if (!m_LockOnActor->ActorHasTag(m_LockOnTag))
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] Lock-on enable tag is not set for the Actor who tried to lock-on."));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}*/
		// 一時的にロックオン出来ないActorであれば処理を終了
		else if (m_LockOnActor == m_CantLockOnActor)
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] This Actor(%s) is temporarily unable to lock on."), *m_LockOnActor->GetName());
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}
		// ロックオン対象が自分自身であれば処理を終了
		else if (m_LockOnActor == GetPlayerCharacter())
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] The player himself is the target of lock-on"));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}
	}
}


// ロックオン状態を解除する関数
void UTPSCameraComponent::DisableLockOn(bool isResetLockOnActor/* = false*/)
{
	m_IsLockOn = false;

	if (isResetLockOnActor)
	{
		m_LockOnActor = nullptr;
	}
}


// 再度ロックオン状態にする関数
bool UTPSCameraComponent::LockOnAgain()
{
	if (m_IsLockOn == false && m_LockOnActor != nullptr)
	{
		m_IsLockOn = true;
		return true;
	}
	else
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Could not reset to lock-on state because LockOnActor is not enabled or already lock-on."));
		return false;
	}
}


// ロックオン位置の取得
FVector UTPSCameraComponent::GetLockOnLocation()
{
	if (m_IsLockOn && m_LockOnActor != nullptr)
	{
		FVector targetOrigin;
		FVector targetExtent;

		m_LockOnActor->GetActorBounds(true, targetOrigin, targetExtent);

		return targetOrigin;
	}
	return FVector::ZeroVector;
}