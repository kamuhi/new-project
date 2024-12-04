/*
	@file	Player.cpp
	@brief	�v���C���[�N���X
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

// �萔
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

	// �f�o�b�O�J�������쐬����
	RECT rect = m_commonResources->GetDeviceResources()->GetOutputSize();
	m_debugCamera = std::make_unique<mylib::DebugCamera>();
	m_debugCamera->Initialize(rect.right, rect.bottom);

	// �ˉe�s����쐬����
	m_projection = SimpleMath::Matrix::CreatePerspectiveFieldOfView(
		XMConvertToRadians(45.0f),
		static_cast<float>(rect.right) / static_cast<float>(rect.bottom),
		0.1f, 100.0f
	);

	// �W�I���g���b�N�v���~�e�B�u�𐶐�����
	m_teapot = GeometricPrimitive::CreateBox(context,Vector3(1.0f));

	// ��]�p������������i�x�j
	m_angle = 0.0f;

	// ���W������������
	m_position = Vector3(-5.5f,0.0f,5.5f);
}

void Player::Update()
{
	// �f�o�b�O�J�������X�V����
	m_debugCamera->Update(m_commonResources->GetInputManager());


	// �L�[�{�[�h�̏�Ԃ��擾����
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	const auto& tracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// ����
	Vector3 velocity = Vector3::Zero;

	//// �O��ړ�
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

	//// ���E��]
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
	
	// �������v�Z����
	velocity *= SPEED;


	// ���W�Ɖ�]�̃��Z�b�g
	if (tracker->IsKeyPressed(Keyboard::Space))
	{
		m_position = Vector3::Zero;
		m_angle = 0.0f;
	}

	// ��]�p�̒l��-180�x<=m_angle<=180�x�Ɏ��߂�
	m_angle = std::min(std::max(m_angle, -180.0f), 180.0f);

	// �ړ��x�N�g�����v�Z���A���W�ɉ��Z����
	Matrix matrix = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	m_position += Vector3::Transform(velocity, matrix);
}

void Player::Render()
{
	auto context = m_commonResources->GetDeviceResources()->GetD3DDeviceContext();

	// �r���[�s����擾����
	Matrix view = m_debugCamera->GetViewMatrix();

	// �L���[�u�̃��[���h�s����v�Z����
	Matrix world = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	world *= Matrix::CreateTranslation(m_position);

	// �W�I���g���b�N�v���~�e�B�u��`�悷��
	m_teapot->Draw(world, view, m_projection, Colors::White);

	// �f�o�b�O�����uDebugString�v�ŕ\������
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("m_angle : %f",m_angle);

}

void Player::Finalize()
{
}
