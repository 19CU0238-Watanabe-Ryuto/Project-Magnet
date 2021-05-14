// 磁力の能力を管理するクラス
//
// 2021/05/12 渡邊龍音 自身の引き寄せ・反発を行う（MagnetComponentBPからの移行）

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MagnetComponent.generated.h"

class UTPSCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UMagnetComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UMagnetComponent();

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

private:

	// TPSCameraComponent
	UTPSCameraComponent* m_TPSCamera;

	// プレイヤーのもともとのgravityScale
	float m_playerOriginGravityScale;

public:
	// 初期化関数
	//
	// 第一引数...キャラクターについているTPSCameraComponent
	void Init(UTPSCameraComponent* _TPSCameraComponent);
};
