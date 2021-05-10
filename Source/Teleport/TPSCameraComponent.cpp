// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"

// Sets default values for this component's properties
UTPSCameraComponent::UTPSCameraComponent()
	: cameraComponent(nullptr)
	, enableLineTrace(true)
	, isDrawDebugLine(true)
	, drawDebugLineTime(0.5f)
	, rayLength(5000.0f)
	, rayOffset(FVector::ZeroVector)
	, isHit(false)
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
	if (!enableLineTrace)
	{
		return;
	}

	// カメラコンポーネント未設定の場合、ログを表示し関数を終了
	if (cameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] camera is null."));
		return;
	}

	// レイ開始位置
	FVector start = cameraComponent->GetComponentLocation();

	// レイ終了位置
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(cameraComponent->GetComponentRotation()) * rayLength;
	FVector end = start + forwardVec + rayOffset;

	// デバッグ確認用のラインを描画
	if (isDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, drawDebugLineTime);
	}

	isHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility);
}


// 初期化関数
void UTPSCameraComponent::Init(UCameraComponent* _camera)
{
	cameraComponent = _camera;
}

