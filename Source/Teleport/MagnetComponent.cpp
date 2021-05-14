// ���͂̔\�͂��Ǘ�����N���X
//
// 2021/05/12 - 5/13 �n粗��� ���g�̈����񂹁E�������s���iMagnetComponentBP����̈ڍs�j

#include "MagnetComponent.h"
#include "TPSCameraComponent.h"

// Sets default values for this component's properties
UMagnetComponent::UMagnetComponent()
	: m_TPSCamera(nullptr)
	, m_IsAttract(false)
	, m_IsFreeze(false)
	, m_IsAttractPlayer(false)
	, m_TargetActorLocation(FVector::ZeroVector)
	, m_AttractPower(10.0f)
	, m_AttractPlayerTagName("")
	, m_AttractObjectTagName("")
	, m_playerOriginGravityScale(0.0f)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called every frame
void UMagnetComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TPSCameraComponent���ݒ肳��Ă��Ȃ���ΏI��
	if (m_TPSCamera == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] TPSCamera is nullptr."));
		return;
	}

	Attract(DeltaTime);
}


// �����񂹏�Ԑ؂�ւ��֐�
void UMagnetComponent::SwitchAttract()
{
	// ���b�N�I����ԂłȂ���ΏI��
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not locked on."));
		return;
	}

	// ���b�N�I���Ώ�Actor���Ȃ���ΏI��
	if (m_TPSCamera->GetIsLockOnActor() == nullptr)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but LockOnActor is nullptr."));
		return;
	}

	// �����񂹏�Ԑ؂�ւ�
	m_IsAttract = !m_IsAttract;

	// �����񂹏�ԂȂ�Actor�̈ʒu�̕ۑ��E�d�͂̕ύX
	if (m_IsAttract)
	{
		// �\�͑Ώۂ��v���C���[���ړ���������̂̏ꍇ
		if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_AttractPlayerTagName))
		{
			// ���b�N�I�������I�u�W�F�N�g�ʒu��ڕW�n�_�Ƃ��ĕۑ�
			m_TargetActorLocation = m_TPSCamera->GetIsLockOnActor()->GetActorLocation();

			// �v���C���[�̏d�͂�0�ɂ���
			m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = 0.0f;

			// �v���C���[�����񂹏�Ԃɂ���
			m_IsAttractPlayer = true;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Player"));
		}
		// �\�͑Ώۂ��I�u�W�F�N�g���ړ���������̂̏ꍇ
		else if (m_TPSCamera->GetIsLockOnActor()->ActorHasTag(m_AttractObjectTagName))
		{
			// �\�͎g�p�n�_�̃v���C���[�̈ʒu��ڕW�n�_�Ƃ��ĕۑ�
			m_TargetActorLocation = m_TPSCamera->GetPlayerCharacter()->GetActorLocation();

			// �v���C���[�����񂹏�Ԃɂ��Ȃ��i�I�u�W�F�N�g�����񂹏�ԁj
			m_IsAttractPlayer = false;

			UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Move Object"));
		}
		// �\�͂̑Ώۂɂ͂Ȃ�Ȃ�
		else
		{

		}
	}
	else
	{
		// �ڕW�n�_�̃��Z�b�g
		m_TargetActorLocation = FVector::ZeroVector;

		// �v���C���[�̏d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
	}
}


// �������֐�
void UMagnetComponent::Init(UTPSCameraComponent* _TPSCameraComponent)
{
	m_TPSCamera = _TPSCameraComponent;
	m_playerOriginGravityScale = m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale;
}


// �����񂹊֐�
void UMagnetComponent::Attract(float _DeltaTime)
{
	// �����񂹏�ԂłȂ���ΏI��
	if (!m_IsAttract)
	{
		// ��~��Ԃ̉���
		m_IsFreeze = false;
		return;
	}

	// ���b�N�I����ԂłȂ���Έ����񂹏�Ԃ��������ďI��
	if (!m_TPSCamera->GetIsLockOn())
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] The Attract button was pressed but not locked on."));

		// �����񂹂�����
		m_IsAttract = false;

		// ��~��Ԃ̉���
		m_IsFreeze = false;

		// �ʒu�̃��Z�b�g
		m_TargetActorLocation = FVector::ZeroVector;

		// �d�͂����ɖ߂�
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->GravityScale = m_playerOriginGravityScale;
		return;
	}

	// �Œ肷���Ԃł����
	if (m_IsFreeze)
	{
		UE_LOG(LogTemp, Verbose, TEXT("[MagnetComponent] Player is freeze."));
		m_TPSCamera->GetPlayerCharacter()->GetCharacterMovement()->Velocity = FVector::ZeroVector;
		return;
	}

	// �v���C���[�������񂹂�
	if (m_IsAttractPlayer)
	{
		// �����񂹂��s��
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetPlayerCharacter()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetPlayerCharacter()->SetActorLocation(interpPos);
	}
	// �I�u�W�F�N�g�������񂹂�
	else
	{
		// �����񂹂��s��
		FVector interpPos = FMath::VInterpTo(m_TPSCamera->GetIsLockOnActor()->GetActorLocation(), m_TargetActorLocation, _DeltaTime, m_AttractPower);
		m_TPSCamera->GetIsLockOnActor()->SetActorLocation(interpPos);
	}

	float length = (m_TPSCamera->GetPlayerCharacter()->GetActorLocation() - m_TargetActorLocation).Size();
	UE_LOG(LogTemp, Warning, TEXT("[MagnetComponent] Actor Length = %f"), length);
}


// �����������ɌĂԊ֐�
void UMagnetComponent::Hit(AActor* _hitActor)
{
	m_IsFreeze = (_hitActor == m_TPSCamera->GetIsLockOnActor());
}