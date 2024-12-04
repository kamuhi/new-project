/*
	@file	PlayScene.h
	@brief	�v���C�V�[���N���X
*/
#pragma once
#include "IScene.h"

// �O���錾
class CommonResources;

namespace mylib
{
	class DebugCamera;
	class GridFloor;
}


class ResultScene final :
    public IScene
{
private:
	// ���ʃ��\�[�X
	CommonResources* m_commonResources;

	// �f�o�b�O�J����
	std::unique_ptr<mylib::DebugCamera> m_debugCamera;

	// �i�q��
	std::unique_ptr<mylib::GridFloor> m_gridFloor;

	// �ˉe�s��
	DirectX::SimpleMath::Matrix m_projection;

	// �V�[���`�F���W�t���O
	bool m_isChangeScene;

	// ���f��
	std::unique_ptr<DirectX::Model> m_model;
	
	// ��]�p(�x)
	int m_angle;


public:
	ResultScene();
	~ResultScene() override;

	void Initialize(CommonResources* resources) override;
	void Update(float elapsedTime)override;
	void Render() override;
	void Finalize() override;

	SceneID GetNextSceneID() const;
};
