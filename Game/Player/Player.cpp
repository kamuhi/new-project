/*
	@file	Player.cpp
	@brief	プレイヤークラス
*/
#include "pch.h"
#include "Player.h"
#include "Game/Bullets/Bullets.h"
#include "Game/CommonResources.h"
#include "DeviceResources.h"
#include "Libraries/MyLib/DebugCamera.h"
#include "Libraries/MyLib/DebugString.h"
#include "Libraries/MyLib/GridFloor.h"
#include "Libraries/MyLib/InputManager.h"
#include "Libraries/MyLib/MemoryLeakDetector.h"
#include <cassert>
#include <algorithm>

#include "Libraries/MyLib/UtilityMath.h"
#include "Game/Screen.h"
using namespace DirectX;
using namespace DirectX::SimpleMath;

// 定数
const float Player::SPEED = 0.2f;

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
	m_position{},
	m_bullets{},
	m_goals{}
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

	//ゴール番号
	m_currentGoalNo = 0;

	// ゴール配列を作成する
	m_goals.emplace_back(Vector3{ 6.0f, 0.0f,  6.0f });
	m_goals.emplace_back(Vector3{ -6.0f, 0.0f,  6.0f });
	m_goals.emplace_back(Vector3{ -6.0f, 0.0f, -6.0f });
	m_goals.emplace_back(Vector3{ 6.0f, 0.0f, -6.0f });

	// 座標を初期化する
	m_position = Vector3{ 5.0f, 0.0f,  5.0f };
	m_spherePosition = Vector3::Zero;

	// ワールド行列
	m_teapotWorld = Matrix::Identity;
	m_sphereWorld = Matrix::Identity;

	// 視界用境界球を初期化する
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = 3.0f;

	// ジオメトリックプリミティブを生成する
	m_teapot = GeometricPrimitive::CreateTeapot(context);

	// 発見フラグを初期化する
	m_isInside = false;

	// 回転角を初期化する（度）
	m_angle = 0.0f;

	m_bullets = std::make_unique<Bullets>();
	m_bullets->Initialize(resources);
}

void Player::Update()
{
	// デバッグカメラを更新する
	m_debugCamera->Update(m_commonResources->GetInputManager());

	// キーボードの状態を取得する
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	const auto& tracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// ティーポットの進行方向ベクトル
	Vector3 heading = Vector3::Transform(Vector3::Forward * 0.25f, Matrix::CreateRotationY(m_angle));

	// ティーポットから現在のゴールへ向かうベクトル
	Vector3 toGoal = m_goals[m_currentGoalNo] - m_position;

	// ゴールまでの距離が近すぎなければ
	if (toGoal.LengthSquared() > SPEED * SPEED)
	{
		// ティーポットを移動する
		m_position += (heading * SPEED);
		m_boundingSphere.Center = m_position;

		// ティーポットがゴールの方向へ徐々に回転する
		float cosTheta = heading.Dot(toGoal) / (toGoal.Length() * heading.Length());
		cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));
		float theta = std::acos(cosTheta);
		theta = std::min(0.1f, theta);

		if (heading.Cross(toGoal).y < 0.0f)
		{
			theta *= (-1.0f);
		}

		// ティーポットの角度を更新する
		m_angle += theta;
	}
	else
	{
		// 現在のゴール番号を保存
		int previousGoalNo = m_currentGoalNo;

		// ゴールに到達したら、次のゴールに移動
		m_currentGoalNo = (m_currentGoalNo + 1) % m_goals.size();

		// 次のゴールへの方向ベクトルを計算
		Vector3 toNextGoal = m_goals[m_currentGoalNo] - m_position;

		// atan2を使用して次のゴールへの角度を計算
		float targetAngle = std::atan2(toNextGoal.x, toNextGoal.z);

		// 角度をラジアンから度に変換
		m_angle = std::abs(XMConvertToDegrees(targetAngle));
		// 角度を-180度から0度の範囲に正規化
		while (m_angle <= -180.0f) m_angle += 180.0f;
		while (m_angle > 180.0f) m_angle -= 360.0f;
	}

	// ティーポットのワールド行列を計算する
	m_teapotWorld = Matrix::CreateRotationY(m_angle);
	m_teapotWorld *= Matrix::CreateTranslation(m_position);

	m_bullets->Update(m_position, m_angle);

	if (kbState.Z)
	{
		m_bullets->SetIsShoot(true);
	}
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

	m_bullets->Render();

	// デバッグ情報を「DebugString」で表示する
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("m_angle : %f",m_angle);

}

void Player::Finalize()
{
}
