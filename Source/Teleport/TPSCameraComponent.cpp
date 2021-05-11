// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる
// 2021/05/11 渡邊龍音 カメラのロックオンを作成

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: m_IsHit(false)
	, m_IsLockOn(false)
	, m_LockOnActor(nullptr)
	, m_CameraComponent(nullptr)
	, m_PlayerCharacter(nullptr)
	, m_EnableLineTrace(true)
	, m_IsDrawDebugLine(true)
	, m_DrawDebugLineTime(0.5f)
	, m_RayLength(5000.0f)
	, m_RayOffset(FVector::ZeroVector)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UTPSCameraComponent::BeginPlay()
{
	Super::BeginPlay();	
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
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] camera is null."));
		return;
	}

	// レイ開始位置
	FVector start = m_CameraComponent->GetComponentLocation();

	// レイ終了位置
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_RayLength;
	FVector end = start + forwardVec + m_RayOffset;

	// デバッグ確認用のラインを描画
	if (m_IsDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, m_DrawDebugLineTime);
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
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not Lock-On."));
		return;
	}

	// ロックオン対象が有効でなければ処理を終了
	if (m_LockOnActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] LockOnActor is nullptr."));
		return;
	}

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
		m_LockOnActor = m_HitResult.GetActor();

		if (m_LockOnActor != nullptr)
		{
			UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] LockOnActor name : %s ."), *m_LockOnActor->GetName());
		}
		else
		{
			UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] LockOnActor is nullptr."));
		}
	}
	else
	{
		m_LockOnActor = nullptr;
	}
}
