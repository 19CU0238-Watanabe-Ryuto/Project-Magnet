// �ǂ𐶐�����N���X
//
// 2021/07/14 �ǂ��v���C���[�̑O�ɐ�������@�\�̂ݎ���
// 2021/07/28 �񐔐�������������

#include "WallSpawnComponent.h"

UWallSpawnComponent::UWallSpawnComponent()
	: m_SpawnCount(0)
	, m_SpawnNum(1)
{
	PrimaryComponentTick.bCanEverTick = true;

	ConstructorHelpers::FObjectFinder<UClass> wallActor(TEXT("/Game/TEST/ReflectionWall/ReflectionWall.ReflectionWall_C"));
	m_WallActor = wallActor.Object;
}


void UWallSpawnComponent::BeginPlay()
{
	Super::BeginPlay();	
}


void UWallSpawnComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

AActor* UWallSpawnComponent::SpawnWall(const FVector pos, const FRotator rot)
{
	if (m_SpawnCount < m_SpawnNum)
	{
		return SpawnActor(m_WallActor, pos, rot);
	}
	return nullptr;
}

// �U�����s��Actor(TSubclassOf<>)�𐶐�
AActor* UWallSpawnComponent::SpawnActor(const TSubclassOf<AActor> _Actor, const FVector _Pos, const FRotator _Rot)
{
	// �����p�����[�^�̐ݒ�
	FActorSpawnParameters params;

	params.bAllowDuringConstructionScript = true;
	params.bDeferConstruction = false;
	params.bNoFail = true;
	params.Instigator = nullptr;
	params.Name = { };
	params.ObjectFlags = EObjectFlags::RF_NoFlags;
	params.OverrideLevel = nullptr;
	params.Owner = GetOwner();
	params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
	params.Template = nullptr;

	// �U��Actor�̐���
	if (_Actor != nullptr)
	{
		m_SpawnCount++;
		return GetWorld()->SpawnActor<AActor>(_Actor, _Pos, _Rot, params);
	}
	else
	{
		return nullptr;
	}
}