/*
	@file	Player.h
	@brief	プレイヤークラス
*/
#pragma once
//前方宣言
class CommonResources;
class Bullets;

namespace mylib
{
	class DebugCamera;
	class GridFloor;
}

class Player
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
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;

	//弾
	std::unique_ptr<Bullets> m_bullets;

	// 回転角（度）
	float m_angle;
	//ゴール番号
	unsigned int m_currentGoalNo;

	//ゴール配列
	std::vector<DirectX::SimpleMath::Vector3> m_goals;
	
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_spherePosition;

	// 各ワールド行列
	DirectX::SimpleMath::Matrix m_teapotWorld;
	DirectX::SimpleMath::Matrix m_sphereWorld;

	// 視界表現用境界球
	DirectX::BoundingSphere m_boundingSphere;

	// 発見フラグ
	bool m_isInside;

	// 速さ
	static const float SPEED;

	//移動制限
	static const float FLOOR_SIZE;

public:
	Player();
	~Player();

	void Initialize(CommonResources* resources);
	void Update();
	void Render();
	void Finalize();

};