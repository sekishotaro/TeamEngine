#pragma once
#include <Windows.h>

//WindowsAPI
class WinApp
{
public: //�萔
	static const int window_width = 1280;	// �E�B���h�E����
	static const int window_height = 720;	// �E�B���h�E�c��

public: //�ÓI�����o�֐�
	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);
public: //�����o�֐�
	//������
	void Initializa();

	//���b�Z�[�W�̏���
	bool ProcessMessage();


	//�I��
	void Finalize();

	//getter
	HWND GetHwnd() { return hwnd; }
	HINSTANCE GetHInstance() { return w.hInstance; }

private:
	//�E�B���h�E�n���h��
	HWND hwnd = nullptr;
	WNDCLASSEX w{}; // �E�B���h�E�N���X�̐ݒ�

};
