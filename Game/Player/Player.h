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

	// 回転角（度）
	float m_angle;
	// 座標
	DirectX::SimpleMath::Vector3 m_position;
	// 速さ
	static const float SPEED;

public:
	Player();
	~Player();

	void Initialize(CommonResources* resources);
	void Update();
	void Render();
	void Finalize();

};