/*
	@file	Player.h
	@brief	プレイヤークラス
*/
#pragma once
//前方宣言
class CommonResources;

namespace mylib
{
	class DebugCamera;
}

class Bullets
{
private:
	//共通リソース
	CommonResources* m_commonResources;

	//デバックカメラ
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;

	//射影行列
	DirectX::SimpleMath::Matrix m_projection;

	//ジオメトリックプリミティブ
	std::unique_ptr<DirectX::GeometricPrimitive> m_teapot;

	// 回転角（度）
	float m_angle;
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	//初速度
	DirectX::SimpleMath::Vector3 m_velocity;

	// 速さ
	static const float SPEED;

	//場外範囲
	static const float FLOOR_SIZE;

	//発射状態
	bool m_IsShoot;

public:
	Bullets();
	~Bullets();

	void Initialize(CommonResources* resources);
	void Update(DirectX::SimpleMath::Vector3 position,float angle);
	void Render();
	void Finalize();

	void SetIsShoot(bool isShoot) { m_IsShoot = isShoot; }

	void Shoot();

};