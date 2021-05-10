// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�

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

	// ���C���g���[�X�����Ȃ��ꍇ�A�֐����I��
	if (!enableLineTrace)
	{
		return;
	}

	// �J�����R���|�[�l���g���ݒ�̏ꍇ�A���O��\�����֐����I��
	if (cameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] camera is null."));
		return;
	}

	// ���C�J�n�ʒu
	FVector start = cameraComponent->GetComponentLocation();

	// ���C�I���ʒu
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(cameraComponent->GetComponentRotation()) * rayLength;
	FVector end = start + forwardVec + rayOffset;

	// �f�o�b�O�m�F�p�̃��C����`��
	if (isDrawDebugLine)
	{
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, drawDebugLineTime);
	}

	isHit = GetWorld()->LineTraceSingleByChannel(hitResult, start, end, ECollisionChannel::ECC_Visibility);
}


// �������֐�
void UTPSCameraComponent::Init(UCameraComponent* _camera)
{
	cameraComponent = _camera;
}

