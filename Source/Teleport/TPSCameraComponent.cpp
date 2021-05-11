// TPS���_�ł̃J�����̐���E�Ǘ����s���N���X
//
// 2021/05/10 �n粗��� �쐬�E���C����ʒ����ɔ�΂������蔻����Ƃ�
// 2021/05/11 �n粗��� �J�����̃��b�N�I�����쐬

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

	// ���C���g���[�X�����Ȃ��ꍇ�A�֐����I��
	if (!m_EnableLineTrace)
	{
		return;
	}

	// �J�����R���|�[�l���g���ݒ�̏ꍇ�A���O��\�����֐����I��
	if (m_CameraComponent == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] camera is null."));
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
		DrawDebugLine(GetWorld(), start, end, FColor::Green, false, m_DrawDebugLineTime);
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
		UE_LOG(LogTemp, Verbose, TEXT("[TPSCameraComponent] Is not Lock-On."));
		return;
	}

	// ���b�N�I���Ώۂ��L���łȂ���Ώ������I��
	if (m_LockOnActor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[TPSCameraComponent] LockOnActor is nullptr."));
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
