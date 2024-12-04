/*
	@file	Player.cpp
	@brief	プレイヤークラス
*/
#include "pch.h"
#include "Player.h"
#include "Game/CommonResources.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/DebugCamera.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/GridFloor.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include <cassert>
#include <algorithm>

using namespace DirectX;
using namespace DirectX::SimpleMath;

// 定数
const float Player::SPEED = 0.05f;

const float Player::FLOOR_SIZE = 5.5f;

Player::Player()
	:
	m_commonResources{},
	//m_basicEffect{},
	//m_primitiveBatch{},
	//m_inputLayout{},
	m_debugCamera{},
	m_projection{},
	m_teapot{},
	m_angle{},
	m_position{}
{
}

Player::~Player()
{
}

void Player::Initialize(CommonResources* resources)
{
	assert(resources);
	m_commonResources = resources;

	auto device = m_commonResources->GetDeviceResources()->GetD3DDevice();
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();
	auto states = m_commonResources->GetCommonStates();

	// デバッグカメラを作成する
	RECT rect = m_commonResources->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// 射影行列を作成する
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 100.0f
	);

	// ジオメトリックプリミティブを生成する
	m_teapot = GeometricPrimitive::CreateBox(context,Vector3(1.0f));

	// 回転角を初期化する（度）
	m_angle = 0.0f;

	// 座標を初期化する
	m_position = Vector3(-5.5f,0.0f,5.5f);
}

void Player::Update()
{
	// デバッグカメラを更新する
	m_debugCamera->Update(m_commonResources->GetInputManager());


	// キーボードの状態を取得する
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	const auto& tracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// 速さ
	Vector3 velocity = Vector3::Zero;

	//// 前後移動
	//if (kbState.Up)
	//{
	//	if(m_position.z >= -FLOOR_SIZE)
	//	velocity = Vector3::Forward;
	//}
	//else if (kbState.Down)
	//{
	//	if(m_position.z <= FLOOR_SIZE)
	//	velocity = Vector3::Backward;
	//}

	//// 左右回転
	//if (kbState.Left)
	//{
	//	//if(m_position.x >= -FLOOR_SIZE)
	//	//velocity = Vector3::Left;
	//	if (kbState.Down)
	//	{
	//		m_angle--;
	//	}
	//	else
	//	{
	//		m_angle++;
	//	}
	//}
	//else if (kbState.Right)
	//{
	//	//if(m_position.x <= FLOOR_SIZE)
	//	//velocity = Vector3::Right;

	//	if (kbState.Down)
	//	{
	//		m_angle++;
	//	}
	//	else
	//	{
	//		m_angle--;
	//	}
	//}
	velocity = Vector3::Forward;
	
	// 速さを計算する
	velocity *= SPEED;


	// 座標と回転のリセット
	if (tracker->IsKeyPressed(Keyboard::Space))
	{
		m_position = Vector3::Zero;
		m_angle = 0.0f;
	}

	// 回転角の値を-180度<=m_angle<=180度に収める
	m_angle = std::min(std::max(m_angle, -180.0f), 180.0f);

	// 移動ベクトルを計算し、座標に加算する
	Matrix matrix = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	m_position += Vector3::Transform(velocity, matrix);
}

void Player::Render()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ビュー行列を取得する
	Matrix view = m_debugCamera->GetViewMatrix();

	// キューブのワールド行列を計算する
	Matrix world = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	world *= Matrix::CreateTranslation(m_position);

	// ジオメトリックプリミティブを描画する
	m_teapot->Draw(world, view, m_projection, Colors::White);

	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("m_angle : %f",m_angle);

}

void Player::Finalize()
{
}
