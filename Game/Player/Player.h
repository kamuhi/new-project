/*
	@file	Player.h
	@brief	�v���C���[�N���X
*/
#pragma once
//�O���錾
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
	
	//���ʃ��\�[�X
	CommonResources* m_commonResources;

	//�f�o�b�N�J����
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;

	//�ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;

	//�W�I���g���b�N�v���~�e�B�u
	std::unique_ptr<DirectX::GeometricPrimitive> m_teapot;
	std::unique_ptr<DirectX::GeometricPrimitive> m_sphere;

	//�e
	std::unique_ptr<Bullets> m_bullets;

	// ��]�p�i�x�j
	float m_angle;
	//�S�[���ԍ�
	unsigned int m_currentGoalNo;

	//�S�[���z��
	std::vector<DirectX::SimpleMath::Vector3> m_goals;
	
	// ���W
	DirectX::SimpleMath::Vector3 m_position;
	DirectX::SimpleMath::Vector3 m_spherePosition;

	// �e���[���h�s��
	DirectX::SimpleMath::Matrix m_teapotWorld;
	DirectX::SimpleMath::Matrix m_sphereWorld;

	// ���E�\���p���E��
	DirectX::BoundingSphere m_boundingSphere;

	// �����t���O
	bool m_isInside;

	// ����
	static const float SPEED;

	//�ړ�����
	static const float FLOOR_SIZE;

public:
	Player();
	~Player();

	void Initialize(CommonResources* resources);
	void Update();
	void Render();
	void Finalize();

};