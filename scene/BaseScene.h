#pragma once

//�O���錾
class SceneManager;

/// <summary>
/// �V�[���C���^�[�t�F�C�X
/// </summary>
class BaseScene
{
public:

	virtual ~BaseScene() = default;

	/// <summary>
	/// ������
	/// </summary>
	virtual void Initialize() = 0;

	/// <summary>
	/// ���
	/// </summary>
	virtual void Finalize() = 0;

	/// <summary>
	/// �A�b�v�f�[�g
	/// </summary>
	virtual void Update() = 0;

	/// <summary>
	/// �`��
	/// </summary>
	virtual void Draw() = 0;
};

