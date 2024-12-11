/*
	@file	Player.cpp
	@brief	�v���C���[�N���X
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

// �萔
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

	//�S�[���ԍ�
	m_currentGoalNo = 0;

	// �S�[���z����쐬����
	m_goals.emplace_back(Vector3{ 6.0f, 0.0f,  6.0f });
	m_goals.emplace_back(Vector3{ -6.0f, 0.0f,  6.0f });
	m_goals.emplace_back(Vector3{ -6.0f, 0.0f, -6.0f });
	m_goals.emplace_back(Vector3{ 6.0f, 0.0f, -6.0f });

	// ���W������������
	m_position = Vector3{ 5.0f, 0.0f,  5.0f };
	m_spherePosition = Vector3::Zero;

	// ���[���h�s��
	m_teapotWorld = Matrix::Identity;
	m_sphereWorld = Matrix::Identity;

	// ���E�p���E��������������
	m_boundingSphere.Center = m_position;
	m_boundingSphere.Radius = 3.0f;

	// �W�I���g���b�N�v���~�e�B�u�𐶐�����
	m_teapot = GeometricPrimitive::CreateTeapot(context);

	// �����t���O������������
	m_isInside = false;

	// ��]�p������������i�x�j
	m_angle = 0.0f;

	m_bullets = std::make_unique<Bullets>();
	m_bullets->Initialize(resources);
}

void Player::Update()
{
	// �f�o�b�O�J�������X�V����
	m_debugCamera->Update(m_commonResources->GetInputManager());

	// �L�[�{�[�h�̏�Ԃ��擾����
	const auto& kbState = m_commonResources->GetInputManager()->GetKeyboardState();
	const auto& tracker = m_commonResources->GetInputManager()->GetKeyboardTracker();

	// �e�B�[�|�b�g�̐i�s�����x�N�g��
	Vector3 heading = Vector3::Transform(Vector3::Forward * 0.25f, Matrix::CreateRotationY(m_angle));

	// �e�B�[�|�b�g���猻�݂̃S�[���֌������x�N�g��
	Vector3 toGoal = m_goals[m_currentGoalNo] - m_position;

	// �S�[���܂ł̋������߂����Ȃ����
	if (toGoal.LengthSquared() > SPEED * SPEED)
	{
		// �e�B�[�|�b�g���ړ�����
		m_position += (heading * SPEED);
		m_boundingSphere.Center = m_position;

		// �e�B�[�|�b�g���S�[���̕����֏��X�ɉ�]����
		float cosTheta = heading.Dot(toGoal) / (toGoal.Length() * heading.Length());
		cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));
		float theta = std::acos(cosTheta);
		theta = std::min(0.1f, theta);

		if (heading.Cross(toGoal).y < 0.0f)
		{
			theta *= (-1.0f);
		}

		// �e�B�[�|�b�g�̊p�x���X�V����
		m_angle += theta;
	}
	else
	{
		// ���݂̃S�[���ԍ���ۑ�
		int previousGoalNo = m_currentGoalNo;

		// �S�[���ɓ��B������A���̃S�[���Ɉړ�
		m_currentGoalNo = (m_currentGoalNo + 1) % m_goals.size();

		// ���̃S�[���ւ̕����x�N�g�����v�Z
		Vector3 toNextGoal = m_goals[m_currentGoalNo] - m_position;

		// atan2���g�p���Ď��̃S�[���ւ̊p�x���v�Z
		float targetAngle = std::atan2(toNextGoal.x, toNextGoal.z);

		// �p�x�����W�A������x�ɕϊ�
		m_angle = std::abs(XMConvertToDegrees(targetAngle));
		// �p�x��-180�x����0�x�͈̔͂ɐ��K��
		while (m_angle <= -180.0f) m_angle += 180.0f;
		while (m_angle > 180.0f) m_angle -= 360.0f;
	}

	// �e�B�[�|�b�g�̃��[���h�s����v�Z����
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

	// �r���[�s����擾����
	Matrix view = m_debugCamera->GetViewMatrix();

	// �L���[�u�̃��[���h�s����v�Z����
	Matrix world = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	world *= Matrix::CreateTranslation(m_position);

	// �W�I���g���b�N�v���~�e�B�u��`�悷��
	m_teapot->Draw(world, view, m_projection, Colors::White);

	m_bullets->Render();

	// �f�o�b�O�����uDebugString�v�ŕ\������
	auto debugString = m_commonResources->GetDebugString();
	debugString->AddString("m_angle : %f",m_angle);

}

void Player::Finalize()
{
}
