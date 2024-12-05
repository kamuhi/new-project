/*
	@file	Player.cpp
	@brief	�v���C���[�N���X
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

// �萔
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
	m_teapot = GeometricPrimitive::CreateSphere(context);

	// ��]�p������������i�x�j
	m_angle = 0.0f;

	// ���W������������
	m_position = Vector3::Zero;

	//
	m_velocity = Vector3::Zero;

	m_IsShoot = false;
}

void Bullets::Update(DirectX::SimpleMath::Vector3 position)
{
	// �f�o�b�O�J�������X�V����
	m_debugCamera->Update(m_commonResources->GetInputManager());


	// �L�[�{�[�h�̏�Ԃ��擾����
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	const auto& tracker = m_commonResources->GetInputManager()->GetKeyboardTracker();


	m_velocity = Vector3::Forward;

	// �������v�Z����
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

	// �r���[�s����擾����
	Matrix view = m_debugCamera->GetViewMatrix();

	// �L���[�u�̃��[���h�s����v�Z����
	//Matrix world = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	Matrix world = Matrix::CreateTranslation(m_position);

	// �W�I���g���b�N�v���~�e�B�u��`�悷��
	m_teapot->Draw(world, view, m_projection, Colors::White);

	// �f�o�b�O�����uDebugString�v�ŕ\������
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("m_angle : %f",m_angle);

}

void Bullets::Finalize()
{
}

void Bullets::Shoot()
{
	//// ����
	//Vector3 velocity = Vector3::Zero;

	//velocity = Vector3::Forward;

	// �������v�Z����
	//m_velocity *= SPEED;

	// �ړ��x�N�g�����v�Z���A���W�ɉ��Z����
	//Matrix matrix = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	m_position += m_velocity;

}
