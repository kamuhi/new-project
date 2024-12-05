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
	m_goals.emplace_back(Vector3{ 5.0f, 0.0f,  5.0f });
	m_goals.emplace_back(Vector3{ -5.0f, 0.0f,  5.0f });
	m_goals.emplace_back(Vector3{ -5.0f, 0.0f, -5.0f });
	m_goals.emplace_back(Vector3{ 5.0f, 0.0f, -5.0f });

	// ���W������������
	m_position = Vector3::Zero;
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
	//velocity = Vector3::Forward;
	//
	//// �������v�Z����
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
	
		// �}�E�X�X�e�[�g���擾����
	const auto& mouseState = m_commonResources->GetInputManager()->GetMouseState();

	/*
		���̍��W���w�肷��
	*/
	//if (mouseState.leftButton)
	{
		// �}�E�X���C���擾����
		Ray mouseRay = mylib::GetMouseRay(mouseState.x, mouseState.y, Screen::WIDTH, Screen::HEIGHT, view, m_projection);

		// ZX�̖������ʂ��`����
		Plane planeZX(0, 1, 0, 0);

		// Ray��Plane�̏Փ˔���
		float distance = 0.0f;
		bool isHit = mouseRay.Intersects(planeZX, distance);

		// Ray��Plane�̏Փˍ��W���g�[���X�̍��W
		if (isHit)
		{
			m_spherePosition = Vector3{ mouseRay.position + mouseRay.direction * distance };
		}
	}

	/*
		�e�B�[�|�b�g���ړ�����
	*/
	// �e�B�[�|�b�g�̐i�s�����x�N�g��
	Vector3 heading = Vector3::Transform(Vector3::Forward * 0.25, Matrix::CreateRotationY(m_angle));

	// �e�B�[�|�b�g����^�[�Q�b�g�֌������x�N�g��
	Vector3 toTarget = m_spherePosition - m_position;

	// �e�B�[�|�b�g����S�[���֌������x�N�g��
	Vector3 toGoal = m_goals[m_currentGoalNo] - m_position;

	// ���E�ɓ���܂ł̍ŒZ����
	float distance = 0.5f + 3.0f;
	// ���ƃe�B�[�|�b�g�Ԃ̋����̕���
	float distSqSphereToTeapot = (m_spherePosition - m_position).LengthSquared();

	// ���E���̃t���O�����Z�b�g����
	m_isInside = false;

	// ���E�͈͓̔����H
	if (distSqSphereToTeapot < distance * distance)
	{
		m_isInside = true;
	}
	else
	{
		// �^�[�Q�b�g�����E�ɓ����Ă��Ȃ��Ƃ��́A�S�[����ڎw��
		toTarget = toGoal;
	}

	// �e�B�[�|�b�g�ƃ^�[�Q�b�g�Ƃ̋������߂����Ȃ����
	if (toTarget.LengthSquared() > SPEED * SPEED)
	{
		// �e�B�[�|�b�g���ړ�����
		m_position += (heading * SPEED);
		m_boundingSphere.Center = m_position;

		/*
			�e�B�[�|�b�g���^�[�Q�b�g�̕����֏��X�ɉ�]����
		*/
		// �u�e�B�[�|�b�g�̐i�s�����x�N�g���v�Ɓu�^�[�Q�b�g�̕����v����cos�Ƃ��v�Z����
		float cosTheta = heading.Dot(toTarget) / (toTarget.Length() * heading.Length());

		// acos�̈����Ŏw��ł���͈͂́u-1�`1�v�Ȃ̂ŁA�l��␳����
		cosTheta = std::max(-1.0f, std::min(cosTheta, 1.0f));

		// cos�Ƃ���Ƃ��v�Z����
		//** acos�̌��ʂ́u0�`�΁v
		float theta = std::acos(cosTheta);

		// �P�t���[���ł̉�]�p�𐧌��l�ȓ��ɕ␳����
		theta = std::min(0.1f, theta);

		// �E���ɍs�������ꍇ�͊p�x�̕�����t���ւ���
		//** ZX���ʏ�ɂ���x�N�g���̌�����Y�̃v���X�}�C�i�X�Ŕ��f����
		if (heading.Cross(toTarget).y < 0.0f)
		{
			theta *= (-1.0f);
		}
		/*
			���e�B�[�|�b�g�ƃ^�[�Q�b�g�̈ʒu�֌W
			�Eheading.Cross(toTorus).y > 0.0f�F�^�[�Q�b�g�̓e�B�[�|�b�g�̍���
			�Eheading.Cross(toTorus).y < 0.0f�F�^�[�Q�b�g�̓e�B�[�|�b�g�̉E��
			�Eheading.Cross(toTorus).y == 0.0f�F�e�B�[�|�b�g�ƃ^�[�Q�b�g�͓������
		*/

		// �e�B�[�|�b�g�̊p�x���X�V����
		m_angle += theta;

		// �S�[���ɒB������A�S�[�������X�V����
		if (m_isInside == false && toTarget.Length() < 0.5f)
		{
			m_currentGoalNo++;
			m_currentGoalNo %= 4;
		}
	}

	// �^�[�Q�b�g�̃��[���h�s����v�Z����
	m_sphereWorld = Matrix::CreateTranslation(m_spherePosition);

	// �e�B�[�|�b�g�̃��[���h�s����v�Z����
	m_teapotWorld = Matrix::CreateRotationY(m_angle);
	m_teapotWorld *= Matrix::CreateTranslation(m_position);

	m_bullets->Update(m_position,m_angle);

	if (kbState.Z)
	{
		m_bullets->SetIsShoot(true);
	}

	//// ��]�p�̒l��-180�x<=m_angle<=180�x�Ɏ��߂�
	//m_angle = std::min(std::max(m_angle, -180.0f), 180.0f);

	//// �ړ��x�N�g�����v�Z���A���W�ɉ��Z����
	//Matrix matrix = Matrix::CreateRotationY(XMConvertToRadians(m_angle));
	//m_position += Vector3::Transform(velocity, matrix);
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
