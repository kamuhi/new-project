/*
	@file	Player.h
	@brief	�v���C���[�N���X
*/
#pragma once
//�O���錾
class CommonResources;

namespace mylib
{
	class DebugCamera;
}

class Bullets
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

	// ��]�p�i�x�j
	float m_angle;
	// ���W
	DirectX::SimpleMath::Vector3 m_position;
	//�����x
	DirectX::SimpleMath::Vector3 m_velocity;

	// ����
	static const float SPEED;

	//��O�͈�
	static const float FLOOR_SIZE;

	//���ˏ��
	bool m_IsShoot;

public:
	Bullets();
	~Bullets();

	void Initialize(CommonResources* resources);
	void Update(DirectX::SimpleMath::Vector3 position,float angle);
	void Render();
	void Finalize();

	void SetIsShoot(bool isShoot) { m_IsShoot = isShoot; }

	void Shoot();

};