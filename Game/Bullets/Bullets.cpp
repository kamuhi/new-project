/*
	@file	Player.cpp
	@brief	プレイヤークラス
*/
#include "pch.h"
#include "Bullets.h"
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
const float Bullets::SPEED = 0.05f;

Bullets::Bullets()
	:
	m_commonResources{},
	//m_basicEffect{},
	//m_primitiveBatch{},
	//m_inputLayout{},
	m_debugCamera{},
	m_projection{},
	m_teapot{},
	m_angle{},
	m_position{},
	m_velocity{},
	m_IsShoot{}
{
}

Bullets::~Bullets()
{
}

void Bullets::Initialize(CommonResources* resources)
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
	m_teapot = GeometricPrimitive::CreateSphere(context);

	// 回転角を初期化する（度）
	m_angle = 0.0f;

	// 座標を初期化する
	m_position = Vector3::Zero;

	//
	m_velocity = Vector3::Zero;

	m_IsShoot = false;
}

void Bullets::Update(DirectX::SimpleMath::Vector3 position)
{
	// デバッグカメラを更新する
	m_debugCamera->Update(m_commonResources->GetInputManager());


	// キーボードの状態を取得する
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	const auto& tracker = m_commonResources->GetInputManager()->GetKeyboardTracker();


	m_velocity = Vector3::Forward;

	// 速さを計算する
	m_velocity *= SPEED;
	if (m_IsShoot == false)
	{
		m_position = position;
	}
	else if (m_IsShoot == true)
	{
		m_position += m_velocity;
	}
	
}

void Bullets::Render()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// ビュー行列を取得する
	Matrix view = m_debugCamera->GetViewMatrix();

	// キューブのワールド行列を計算する
	//Matrix world = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	Matrix world = Matrix::CreateTranslation(m_position);

	// ジオメトリックプリミティブを描画する
	m_teapot->Draw(world, view, m_projection, Colors::White);

	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("m_angle : %f",m_angle);

}

void Bullets::Finalize()
{
}

void Bullets::Shoot()
{
	//// 速さ
	//Vector3 velocity = Vector3::Zero;

	//velocity = Vector3::Forward;

	// 速さを計算する
	//m_velocity *= SPEED;

	// 移動ベクトルを計算し、座標に加算する
	//Matrix matrix = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	m_position += m_velocity;

}
