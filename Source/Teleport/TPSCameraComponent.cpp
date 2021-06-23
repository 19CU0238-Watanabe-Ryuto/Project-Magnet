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
// 2021/06/02 渡邊龍音 ロックオン不可のActorを複数設定できる様に
//					   SwitchLockOn関数でロックオンしたActorを返すように
//					   ロックオン可能な一番近いActor（m_NearCanLockOnActor）を取得できるように
// 2021/06/04 渡邊龍音 ロックオン時に断定的に画角を下げる

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Components/BoxComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: m_BoxComponent(nullptr)
	, m_RayHitLocation(FVector::ZeroVector)
	, m_IsHit(false)
	, m_IsHitCanLockOnActor(false)
	, m_CollisionParams(FCollisionQueryParams::DefaultQueryParam)
	, m_IsLockOn(false)
	, m_LockOnActor(nullptr)
	, m_CameraComponent(nullptr)
	, m_PlayerCharacter(nullptr)
	, m_NearCanLockOnActor(nullptr)
	, m_NowFOV(90.0f)
	, m_LockOnTimer(0.0f)
	, m_DisableLockOnLength(0.0f)
	, m_FOVSmoothTime(0.2f)
	, m_NormalFOV(90.0f)
	, m_LockOnFOV(50.0f)
	, m_LockOnTag("CanLockOn")
	, m_BoxCollisionSize(FVector(7500.0f, 100.0f, 100.0f))
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UTPSCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (m_BoxComponent != nullptr)
	{
		// コリジョンを強制的に毎フレーム有効にする
		m_BoxComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		m_BoxComponent->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] BoxComponent is null."))
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
	// ロックオン処理
	LockOn(DeltaTime);
}


void UTPSCameraComponent::OnOverlapBegin(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor->ActorHasTag(m_LockOnTag))
	{
		m_OnCollisionCanLockOnActorArray.AddUnique(OtherActor);
	}
}


void UTPSCameraComponent::OnComponentOverlapEnd(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor->ActorHasTag(m_LockOnTag))
	{
		m_OnCollisionCanLockOnActorArray.Remove(OtherActor);
	}
}


// ロックオン処理関数
void UTPSCameraComponent::LockOn(float DeltaTime)
{
	FVector start = m_CameraComponent->GetComponentLocation();
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_BoxCollisionSize.X;
	FVector end = start + forwardVec;

	m_CameraVector = end - start;

	// レイを飛ばす
	DrawDebugLine(GetWorld(), start, end, FColor::Green, false, DeltaTime);
	m_IsHit = GetWorld()->LineTraceSingleByChannel(m_HitResult, start, end, ECollisionChannel::ECC_Pawn, m_CollisionParams);

	// カメラからのレイが当たった場所を保存
	if (m_IsHit)
	{
		m_RayHitLocation = m_HitResult.Location;
	}
	else
	{
		m_RayHitLocation = end;
	}

	// 一番近いActorをリセットする
	m_NearCanLockOnActor = nullptr;


	// 一番近いActorがどれか検索
	float nearLength = m_BoxCollisionSize.X;

	// 保存しているActorの個数分ループする
	for (AActor* nearActor : m_OnCollisionCanLockOnActorArray)
	{
		if (!nearActor)
		{
			continue;
		}

		if (!m_PlayerCharacter)
		{
			continue;
		}
				
		if (nearActor == GetPlayerCharacter())
		{
			continue;
		}

		// Actorのバウンディングボックスを取得
		FVector bound = FVector::ZeroVector;
		FVector ex = FVector::ZeroVector;

		nearActor->GetActorBounds(true, bound, ex);

		// Actorとの距離を計測
		float length = (bound - m_PlayerCharacter->GetActorLocation()).Size();

		/*
		// Actorとの角度を算出
		FVector cameraVec = m_CameraComponent->GetForwardVector();
		FRotator objectRot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), bound);
		FVector objectVec = UKismetMathLibrary::GetForwardVector(objectRot);

		float distance = FVector::Dist(cameraVec, objectVec);
		float deg = UKismetMathLibrary::DegAcos(distance);

		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Degree = %f Actor = %s"), deg, *lockOnActor->GetName());

		if (deg > m_CanLockOnDegree)
		{

		}
		*/

		bool cantLockOn = false;

		for (AActor* ignore : m_CantLockOnActorArray)
		{
			// 一時的にロックオン出来ないActorであれば処理を終了
			if (nearActor == ignore)
			{
				cantLockOn = true;
				break;
			}
		}

		if (!cantLockOn)
		{
			// カメラからのレイが当たっていれば
			if (m_IsHit)
			{
				// カメラのレイの当たった位置からActorまでレイを飛ばす
				FHitResult res;

				// レイの衝突をお互いに当たらないようにする
				FCollisionQueryParams colParams;
				colParams.AddIgnoredActor(m_PlayerCharacter);
				colParams.AddIgnoredActor(nearActor);

				bool isHit = GetWorld()->LineTraceSingleByChannel(res, bound, m_RayHitLocation, ECollisionChannel::ECC_Pawn, colParams);

				// ロックオン可能範囲でかつ、カメラの角度が範囲内で、さらにレイが当たっていなければ（遮るものがない）
				if (length < nearLength && !isHit)
				{
					m_NearCanLockOnActor = nearActor;
					nearLength = length;
				}
			}
			else
			{
				m_NearCanLockOnActor = nearActor;
				nearLength = length;
			}
		}		
	}
	m_LockOnTimer += DeltaTime;

	if (m_LockOnTimer > m_FOVSmoothTime)
	{
		m_LockOnTimer = m_FOVSmoothTime;
	}

	// ロックオン状態でなければ処理を終了
	if (!m_IsLockOn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not lock-On."));

		m_NowFOV = FMath::Lerp(m_LockOnFOV, m_NormalFOV, m_LockOnTimer / m_FOVSmoothTime);
		m_CameraComponent->FieldOfView = m_NowFOV;

		return;
	}
	else
	{
		m_NowFOV = FMath::Lerp(m_NormalFOV, m_LockOnFOV, m_LockOnTimer / m_FOVSmoothTime);
		m_CameraComponent->FieldOfView = m_NowFOV;
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
	float radius;

	UKismetSystemLibrary::GetComponentBounds(m_LockOnActor->GetRootComponent(), targetOrigin, targetExtent, radius);

	//m_LockOnActor->GetActorBounds(true, targetOrigin, targetExtent);

	FVector playerPos(m_PlayerCharacter->GetActorLocation().X, m_PlayerCharacter->GetActorLocation().Y, 0.0f);
	FVector targetPos(m_LockOnActor->GetActorLocation().X, m_LockOnActor->GetActorLocation().Y, 0.0f);
	float targetDistance = (playerPos - targetPos).Size();

	// ロックオンしているオブジェクトとの距離がレイの距離以上になったらロックオン状態解除
	if (targetDistance >= m_BoxCollisionSize.X)
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

	if (m_PlayerCharacter != nullptr && m_PlayerCharacter->GetController() != nullptr)
	{
		//m_PlayerCharacter->GetController()->SetControlRotation(rot);
	}
}


// m_CameraVector取得用（正規化）
FVector UTPSCameraComponent::GetCameraVectorOtherActor(FVector _originPos)
{
	if (m_CameraComponent != nullptr)
	{
		// レイ終了位置
		FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation());
		FVector end = _originPos + forwardVec;

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
void UTPSCameraComponent::Init(UCameraComponent* _camera, ACharacter* _character, UBoxComponent* _boxConponent)
{
	m_CameraComponent = _camera;
	m_PlayerCharacter = _character;
	m_BoxComponent = _boxConponent;

	m_LockOnTimer = m_FOVSmoothTime;

	// レイの衝突を自分自身のキャラクターに当たらないようにする
	m_CollisionParams.AddIgnoredActor(_character);

	if (m_BoxComponent != nullptr)
	{
		FVector tmpSize = m_BoxCollisionSize;
		tmpSize.X = m_BoxCollisionSize.X / 2.0f;

		FVector tmpLoc = m_BoxComponent->GetRelativeLocation();
		tmpLoc.X += m_BoxCollisionSize.X / 2.0f;

		m_BoxComponent->SetBoxExtent(tmpSize);
		m_BoxComponent->SetRelativeLocation(tmpLoc);

		m_BoxComponent->OnComponentBeginOverlap.AddDynamic(this, &UTPSCameraComponent::OnOverlapBegin);
		m_BoxComponent->OnComponentEndOverlap.AddDynamic(this, &UTPSCameraComponent::OnComponentOverlapEnd);
	}

	m_OnCollisionCanLockOnActorArray.Empty();
}


// ロックオン状態にする関数
AActor* UTPSCameraComponent::SwitchLockOn()
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
			return nullptr;
		}
		// ロックオン出来ないActorであれば処理を終了
		/*
		else if (!m_LockOnActor->ActorHasTag(m_LockOnTag))
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] Lock-on enable tag is not set for the Actor who tried to lock-on."));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}*/

		m_LockOnTimer = m_FOVSmoothTime - m_LockOnTimer;

		/*
		// ロックオン対象が自分自身であれば処理を終了
		else if (m_LockOnActor == GetPlayerCharacter())
		{
			UE_LOG(LogTemp, Log, TEXT("[TPSCameraComponent] The player himself is the target of lock-on"));
			m_LockOnActor = nullptr;
			m_IsLockOn = false;
		}*/
	}
	else
	{
		m_LockOnTimer = m_FOVSmoothTime - m_LockOnTimer;
	}

	return m_LockOnActor;
}


// ロックオン状態を解除する関数
void UTPSCameraComponent::DisableLockOn(bool isResetLockOnActor/* = false*/)
{
	if (m_IsLockOn)
	{
		m_LockOnTimer = m_FOVSmoothTime - m_LockOnTimer;
	}
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