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
const float Player::SPEED = 0.1f;

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
	m_goals.emplace_back(Vector3{ 5.0f, 0.0f,  5.0f });
	m_goals.emplace_back(Vector3{ -5.0f, 0.0f,  5.0f });
	m_goals.emplace_back(Vector3{ -5.0f, 0.0f, -5.0f });
	m_goals.emplace_back(Vector3{ 5.0f, 0.0f, -5.0f });

	// 座標を初期化する
	m_position = Vector3::Zero;
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
	//velocity = Vector3::Forward;
	//
	//// 速さを計算する
	//velocity *= SPEED;

	//if (m_position.z <= -FLOOR_SIZE)
	//{
	//	m_angle = -90.0f;
	//}
	////else if (m_position.z >= FLOOR_SIZE)
	////{
	////	m_angle = -270.0f;
	////}

	//if (m_position.x >= FLOOR_SIZE)
	//{
	//	m_angle = -180.0f;
	//}

	////else if (m_position.z >= 6.0f)
	////{
	////	m_angle - 90.0f;
	////}
	Vector3 eye{ 0.0f,15.0f,0.0f };
	Vector3 target = Vector3::Zero;
	Vector3 up = -Vector3::UnitZ;
	DirectX::SimpleMath::Matrix view;
	view = Matrix::CreateLookAt(eye, target, up);
	
		// マウスステートを取得する
	const auto& mouseState = m_commonResources->GetInputManager()->GetMouseState();

	/*
		球の座標を指定する
	*/
	//if (mouseState.leftButton)
	{
		// マウスレイを取得する
		Ray mouseRay = mylib::GetMouseRay(mouseState.x, mouseState.y, Screen::WIDTH, Screen::HEIGHT, view, m_projection);

		// ZXの無限平面を定義する
		Plane planeZX(0, 1, 0, 0);

		// RayとPlaneの衝突判定
		float distance = 0.0f;
		bool isHit = mouseRay.Intersects(planeZX, distance);

		// RayとPlaneの衝突座標→トーラスの座標
		if (isHit)
		{
			m_spherePosition = Vector3{ mouseRay.position + mouseRay.direction * distance };
		}
	}

	/*
		ティーポットを移動する
	*/
	// ティーポットの進行方向ベクトル
	Vector3 heading = Vector3::Transform(Vector3::Forward * 0.25, Matrix::CreateRotationY(m_angle));

	// ティーポットからターゲットへ向かうベクトル
	Vector3 toTarget = m_spherePosition - m_position;

	// ティーポットからゴールへ向かうベクトル
	Vector3 toGoal = m_goals[m_currentGoalNo] - m_position;

	// 視界に入るまでの最短距離
	float distance = 0.5f + 3.0f;
	// 球とティーポット間の距離の平方
	float distSqSphereToTeapot = (m_spherePosition - m_position).LengthSquared();

	// 視界内のフラグをリセットする
	m_isInside = false;

	// 視界の範囲内か？
	if (distSqSphereToTeapot < distance * distance)
	{
		m_isInside = true;
	}
	else
	{
		// ターゲットが視界に入っていないときは、ゴールを目指す
		toTarget = toGoal;
	}

	// ティーポットとターゲットとの距離が近すぎなければ
	if (toTarget.LengthSquared() > SPEED * SPEED)
	{
		// ティーポットを移動する
		m_position += (heading * SPEED);
		m_boundingSphere.Center = m_position;

		/*
			ティーポットがターゲットの方向へ徐々に回転する
		*/
		// 「ティーポットの進行方向ベクトル」と「ターゲットの方向」からcosθを計算する
		float cosTheta = heading.Dot(toTarget) / (toTarget.Length() * heading.Length());

		// acosの引数で指定できる範囲は「-1～1」なので、値を補正する
		cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));

		// cosθからθを計算する
		//** acosの結果は「0～π」
		float theta = std::acos(cosTheta);

		// １フレームでの回転角を制限値以内に補正する
		theta = std::min(0.1f, theta);

		// 右側に行きたい場合は角度の符号を付け替える
		//** ZX平面上にあるベクトルの向きはYのプラスマイナスで判断する
		if (heading.Cross(toTarget).y < 0.0f)
		{
			theta *= (-1.0f);
		}
		/*
			■ティーポットとターゲットの位置関係
			・heading.Cross(toTorus).y > 0.0f：ターゲットはティーポットの左側
			・heading.Cross(toTorus).y < 0.0f：ターゲットはティーポットの右側
			・heading.Cross(toTorus).y == 0.0f：ティーポットとターゲットは同一線上
		*/

		// ティーポットの角度を更新する
		m_angle += theta;

		// ゴールに達したら、ゴール情報を更新する
		if (m_isInside == false && toTarget.Length() < 0.5f)
		{
			m_currentGoalNo++;
			m_currentGoalNo %= 4;
		}
	}

	// ターゲットのワールド行列を計算する
	m_sphereWorld = Matrix::CreateTranslation(m_spherePosition);

	// ティーポットのワールド行列を計算する
	m_teapotWorld = Matrix::CreateRotationY(m_angle);
	m_teapotWorld *= Matrix::CreateTranslation(m_position);

	m_bullets->Update(m_position,m_angle);

	if (kbState.Z)
	{
		m_bullets->SetIsShoot(true);
	}

	//// 回転角の値を-180度<=m_angle<=180度に収める
	//m_angle = std::min(std::max(m_angle, -180.0f), 180.0f);

	//// 移動ベクトルを計算し、座標に加算する
	//Matrix matrix = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	//m_position += Vector3::Transform(velocity, matrix);
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
