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

	// ��]�p�i�x�j
	float m_angle;
	// ���W
	DirectX::SimpleMath::Vector3 m_position;
	// ����
	static const float SPEED;

public:
	Player();
	~Player();

	void Initialize(CommonResources* resources);
	void Update();
	void Render();
	void Finalize();

};