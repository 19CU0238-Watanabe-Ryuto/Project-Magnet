// ��������I�u�W�F�N�g�������̃N���X
//
// 2021/05/26 �n粗��� �����o�������̊֐����쐬
// 2021/05/30 �n粗��� �N�������o�����̂�������悤��
// 2021/06/01 �n粗��� �N�������o�����̂� ->�N���������Ă���Ƃ��ɒN�������Ă���̂�������悤��
// 2021/06/04 �n粗��� �����Ă���Actor���O�����烊�Z�b�g����֐���ǉ�
// 2021/06/09 �n粗��� ���˂̏�����ǉ�

#include "ItemShootComponent.h"
#include "Kismet/KismetMathLibrary.h"
#include "DrawDebugHelpers.h"
#include "GameFramework/ProjectileMovementComponent.h"

// Sets default values for this component's properties
UItemShootComponent::UItemShootComponent()
	: m_StaticMeshComp(nullptr)
	, m_ProjectileMovement(nullptr)
	, m_IsShoot(false)
	, m_WhoHave(nullptr)
	, m_ShootTargetActor(nullptr)
	, m_BeginShootLocation(FVector::ZeroVector)
	, m_ShootTargetLocation(FVector::ZeroVector)
	, m_NowDamage(0)
	, m_ShootPower(0.0f)
	, m_TargetLength(0.0f)
	, m_InitialHitDamage(200)
	, m_ActorResetFlg(false)
{
	PrimaryComponentTick.bCanEverTick = true;
}


// Called when the game starts
void UItemShootComponent::BeginPlay()
{
	Super::BeginPlay();

	if (GetOwner() != nullptr)
	{
		m_StaticMeshComp = Cast<UStaticMeshComponent>(GetOwner()->GetRootComponent());
		m_ProjectileMovement = Cast<UProjectileMovementComponent>(GetOwner()->GetComponentByClass(UProjectileMovementComponent::StaticClass()));
	}

	if (m_StaticMeshComp == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[ItemShootComponent] The RootComponent of the Actor that can be attracted must be a StaticMeshComponent."))
	}

	if (m_ProjectileMovement == nullptr)
	{
		FString name = GetOwner() != nullptr ? GetOwner()->GetName() : "Could Not Get Actor";
		UE_LOG(LogTemp, Warning, TEXT("[ItemShootComponent] If this is ball, Need add ProjectileMovementComponent. (This Actor Name = %s)"), *name);
	}
}


// Called every frame
void UItemShootComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	Shoot(DeltaTime);

	if (m_ActorResetFlg && !m_IsShoot)
	{
		Reset(true);
	}
}


// ������Ƃ��̊֐�
UFUNCTION(BlueprintCallable)
void UItemShootComponent::GetItem(AActor* _getActor)
{
	m_WhoHave = _getActor;

	m_ActorResetFlg = false;
}


// �����͂��߂�֐�
void UItemShootComponent::BeginShoot(FVector _shootPos, float _shootPower, FVector _targetPos, AActor* _targetActor/* = nullptr*/)
{
	m_IsShoot = true;
	m_NowDamage = m_InitialHitDamage;

	m_BeginShootLocation = _shootPos;
	m_ShootPower = _shootPower;
	m_ShootTargetLocation = _targetPos;
	m_ShootTargetActor = _targetActor;

	m_TargetLength = (m_BeginShootLocation + m_ShootTargetLocation).Size2D();

	//if (m_ProjectileMovement != nullptr)
	{
		FVector vector = m_ShootTargetLocation - m_StaticMeshComp->GetComponentLocation();
		vector.Normalize();

		m_StaticMeshComp->AddImpulse(vector * m_ShootPower);
	}

	m_ActorResetFlg = true;
}


// �������Ƃ��̊֐�
void UItemShootComponent::Shoot(float _time)
{
	// ���ˏ�Ԃł����
	if (m_IsShoot)
	{
		// ���ˈʒu�ƖړI�n�����񂾐��ɒ�������A�I�u�W�F�N�g����L�΂������̒������v�Z
		float orthogonalLineLength = FMath::Abs((m_ShootTargetLocation + m_BeginShootLocation).Size2D());
		if (m_ShootTargetActor != nullptr)
		{
			// ���ˈʒu��0 �ړI�n��1�Ƃ����Ƃ��ɂǂ��ɂ��邩�v�Z
			// ���ˈʒu����ڕW�n�_�̃x�N�g���ƁA���ˈʒu����I�u�W�F�N�g�̃x�N�g�����Ȃ�cos�����߂�
			// �x�N�g�������߂�
			FVector targetVector = m_ShootTargetLocation - m_BeginShootLocation;
			FVector objectVector = m_StaticMeshComp->GetComponentLocation() - m_BeginShootLocation;

			// cos�����߂�
			float dividend = (targetVector.X * objectVector.X + targetVector.Y * objectVector.Y);
			float divitor = FMath::Sqrt(FMath::Pow(targetVector.X, 2) + FMath::Pow(targetVector.Y, 2)) * FMath::Sqrt(FMath::Pow(objectVector.X, 2) + FMath::Pow(objectVector.Y, 2));
			float objectCos = dividend / divitor;

			// ���ˈʒu����I�u�W�F�N�g�̋��������߂�
			float objectLength = (m_BeginShootLocation + m_StaticMeshComp->GetComponentLocation()).Size2D();
			
			// �ΕӁi���ˈʒu����I�u�W�F�N�g�̋����j��cos���A���ˈʒu����I�u�W�F�N�g�̋��������߂�
			float length = objectLength * objectCos;

			UE_LOG(LogTemp, Warning, TEXT("[ItemShootComponent] Target Length = %f, Pass Object Length = %f, Ratio = %f"), m_TargetLength, length, (length / m_TargetLength));

			FVector homingVector = m_ShootTargetActor->GetActorLocation() - m_StaticMeshComp->GetComponentLocation();
			homingVector.Normalize();

			m_StaticMeshComp->AddForce(homingVector * m_ShootPower);
		}
	}
}


// ���������̌v�Z
int UItemShootComponent::AttenuationDamage(FVector _hitPos, float _attenuationDistance/* = 100.0f */, int _attenuationAmount/* = 1 */)
{
	float length = (_hitPos - m_BeginShootLocation).Size();

	if (_attenuationDistance == 0.0f)
	{
		_attenuationDistance = length;
	}

	m_NowDamage -= (length / _attenuationDistance * _attenuationAmount);

	return m_NowDamage;
}


// �ǂɂ����������Ɍ����o���ʒu�E�З͂����X�V����֐�
int UItemShootComponent::HitAttenuationDamage(FVector _hitPos, float _attenuationRatio/* = 0.75f */, float _attenuationDistance/* = 100.0f */, int _attenuationAmount/* = 1 */)
{
	AttenuationDamage(_hitPos, _attenuationDistance, _attenuationAmount);
	m_BeginShootLocation = _hitPos;

	m_NowDamage *= _attenuationRatio;

	return m_NowDamage;
}


void UItemShootComponent::Reset(bool _resetActor /* = true */)
{
	m_ActorResetFlg = false;

	if (_resetActor)
	{
		m_WhoHave = nullptr;
	}
}