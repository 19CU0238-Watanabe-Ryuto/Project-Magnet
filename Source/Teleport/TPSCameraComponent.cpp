// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�iTPSCameraComponentBP����̈ڍs�j
// 2021/05/11 �n粗��� �J�����̃��b�N�I�����쐬
// 2021/05/12 �n粗��� ���b�N�I���Ώۂƈ��̋����ȓ��ɓ������ꍇ�A���b�N�I������������

#include "TPSCameraComponent.h"
#include "Camera/CameraComponent.h"
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
	, m_LockOnRayColor(FColor::Red)
	, m_NoLockOnRayColor(FColor::Green)
	, m_RayLength(5000.0f)
	, m_RayOffset(FVector::ZeroVector)
	, m_DisableLockOnLength(100.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UTPSCameraComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ���C���g���[�X�����Ȃ��ꍇ�A�֐����I��
	if (!m_EnableLineTrace)
	{
		return;
	}

	// �J�����R���|�[�l���g���ݒ�̏ꍇ�A���O��\�����֐����I��
	if (m_CameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] Camera is null."));
		return;
	}

	// �v���C���[�R���|�[�l���g���ݒ�̏ꍇ�A���O��\�����֐����I��
	if (m_PlayerCharacter == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] PlayerCharacter is null."));
		return;
	}

	// ���C�J�n�ʒu
	FVector start = m_CameraComponent->GetComponentLocation();

	// ���C�I���ʒu
	FVector forwardVec = UKismetMathLibrary::GetForwardVector(m_CameraComponent->GetComponentRotation()) * m_RayLength;
	FVector end = start + forwardVec + m_RayOffset;

	// �f�o�b�O�m�F�p�̃��C����`��
	if (m_IsDrawDebugLine)
	{
		FColor lineColor = m_IsLockOn ? m_LockOnRayColor : m_NoLockOnRayColor;
		DrawDebugLine(GetWorld(), start, end, lineColor, false, m_DrawDebugLineTime);
	}

	// ���C���΂������蔻������
	m_IsHit = GetWorld()->LineTraceSingleByChannel(m_HitResult, start, end, ECollisionChannel::ECC_Visibility);

	// ���b�N�I������
	LockOn();
}


// ���b�N�I�������֐�
void UTPSCameraComponent::LockOn()
{
	// ���b�N�I����ԂłȂ���Ώ������I��
	if (!m_IsLockOn)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not lock-On."));
		return;
	}

	// ���b�N�I���Ώۂ��L���łȂ���Ώ������I��
	if (m_LockOnActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] LockOnActor is nullptr."));
		return;
	}

	FVector playerPos(m_PlayerCharacter->GetActorLocation().X, m_PlayerCharacter->GetActorLocation().Y, 0.0f);
	FVector targetPos(m_LockOnActor->GetActorLocation().X, m_LockOnActor->GetActorLocation().Y, 0.0f);
	float targetDistance = (playerPos - targetPos).Size();

	// ���b�N�I���ΏۂƂ̋������ݒ�l�ȉ��ł���΃��b�N�I����Ԃ��������������I��
	if (targetDistance < m_DisableLockOnLength)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Player got within a certain distance, so player disabled lock-on."));
		//m_IsLockOn = false;
		return;
	}

	FVector targetLocation = m_LockOnActor->GetActorLocation();
	FRotator rot = UKismetMathLibrary::FindLookAtRotation(m_CameraComponent->GetComponentLocation(), targetLocation);

	m_PlayerCharacter->GetController()->SetControlRotation(rot);
}


// �������֐�
void UTPSCameraComponent::Init(UCameraComponent* _camera, ACharacter* _character)
{
	m_CameraComponent = _camera;
	m_PlayerCharacter = _character;
}


// ���b�N�I����Ԃɂ���֐�
void UTPSCameraComponent::SwitchLockOn()
{
	// ���b�N�I����Ԃ̃X�C�b�`
	m_IsLockOn = !m_IsLockOn;

	if (m_IsLockOn)
	{
		m_LockOnActor = m_HitResult.GetActor();

		if (m_LockOnActor == nullptr)
		{
			m_IsLockOn = false;
		}
	}
	else
	{
		m_LockOnActor = nullptr;
	}
}
