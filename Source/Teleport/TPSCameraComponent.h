// TPS視点でのカメラの制御・管理を行うクラス
//
// 2021/05/10 渡邊龍音 作成・レイを画面中央に飛ばし当たり判定をとる

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "TPSCameraComponent.generated.h"

class UCameraComponent;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class TELEPORT_API UTPSCameraComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTPSCameraComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// カメラコンポーネント
	UPROPERTY(EditAnyWhere)
		UCameraComponent* cameraComponent;

	// ライントレースを有効にするか（レイを飛ばすか）
	UPROPERTY(EditAnyWhere)
		bool enableLineTrace;

	// デバッグ用にレイを描画するか
	UPROPERTY(EditAnyWhere, Category = "Debug")
		bool isDrawDebugLine;

	// デバッグ用レイの表示時間
	UPROPERTY(EditAnyWhere, Category = "Debug")
		float drawDebugLineTime;

	// レイの距離
	UPROPERTY(EditAnyWhere)
		float rayLength;

	// レイのオフセット位置
	UPROPERTY(EditAnyWhere)
		FVector rayOffset;

	// レイがほかのActorにあたっているか
	UPROPERTY(VisibleAnyWhere)
		bool isHit;

	// レイが当たった結果
	UPROPERTY(VisibleAnyWhere)
		FHitResult hitResult;

public:
	// 初期化関数
	// 
	// 第一引数 カメラコンポーネント
	UFUNCTION(BlueprintCallable)
		void Init(UCameraComponent* _camera);
};
