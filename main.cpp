//=============================================================================
//
// ���C������ [main.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "renderer.h"
#include "object.h"
#include "object2D.h"
#include <stdlib.h>
#include <crtdbg.h>
#include "manager.h"
#include <tchar.h>
#include <stdio.h>
#include "imguimaneger.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

//=============================================================================
// ���C���֐�
//=============================================================================
int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE /*hInstancePrev*/, _In_ LPSTR /*lpCmdLine*/, _In_ int /*nCmdShow*/)
{
	// ���������[�N���m
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF | _CRTDBG_CHECK_ALWAYS_DF);
	//_CrtSetBreakAlloc(12455);// ���[�N�̌����̃C���X�^���X�����ɋ����I�Ƀu���[�N�|�C���g�𔭐�������

	int nCountFPS = 0;// FPS�J�E���^�[

	CManager* pManager = nullptr;

	// �}�l�[�W���[�̐���
	pManager = new CManager;

	pManager->GetFPS(nCountFPS);

	DWORD dwCurrentTime;// ���ݎ���
	DWORD dwExecLastTime;// �Ō�ɏ�����������

	WNDCLASSEX wcex =
	{
		sizeof(WNDCLASSEX),             // WNDCLASSEX�̃������T�C�Y
		CS_CLASSDC,                     // �E�B���h�E�̃X�^�C��
		WindowProc,                     // �E�B���h�E�v���V�[�W��
		0,                              // 0�ɂ���(�ʏ�͎g�p���Ȃ�)
		0,                              // 0�ɂ���
		hInstance,                      // �C���X�^���X�n���h��
		LoadIcon(NULL,IDI_APPLICATION), // �^�X�N�o�[�̃A�C�R��
		LoadCursor(NULL,IDC_ARROW),     // �}�E�X�J�[�\��
		(HBRUSH)(COLOR_WINDOW + 1),     // �N���C�A���g�̈�̔w�i�F
		NULL,                           // ���j���[�o�[
		CLASS_NAME,                     // �E�B���h�E�N���X�̖��O
		LoadIcon(NULL,IDI_APPLICATION)  // �t�@�C���̃A�C�R��
	};


	HWND hWnd;   // �E�B���h�E�n���h��
	MSG msg;     // ���b�Z�[�W���i�[����ϐ�

	RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	// �E�B���h�E�N���X�̓o�^
	RegisterClassEx(&wcex);

	// �N���C�A���g�̈���w��̃T�C�Y�ɒ���
	AdjustWindowRect(&rect, WS_OVERLAPPEDWINDOW, FALSE);

	// �E�B���h�E�𐶐�
	hWnd = CreateWindowEx(0,    // �g���E�B���h�E�X�^�C��
		CLASS_NAME,             // �E�B���h�E�N���X�̖��O
		WINDOW_NAME,            // �E�B���h�E�̖��O
		WS_OVERLAPPEDWINDOW,    // �E�B���h�E�X�^�C��
		CW_USEDEFAULT,          // �E�B���h�E�̍���x���W
		CW_USEDEFAULT,          // �E�B���h�E�̍���y���W

        (rect.right-rect.left), // ��
		(rect.bottom-rect.top), // ����

		NULL,                   // �e�E�B���h�E�̃n���h��
		NULL,                   // ���j���[�n���h���܂��͎q�E�B���h�EID
		hInstance,              // �C���X�^���X�n���h��
		NULL);                  // �E�B���h�E�쐬�f�[�^


	// �}�l�[�W���[�̏���������
	if (FAILED(pManager->Init(hInstance, hWnd)))
	{
		return -1;
	}

	// ����\��ݒ�
	timeBeginPeriod(1);
	dwCurrentTime = 0;					// ����������
	dwExecLastTime = timeGetTime();		// ���ݎ������擾�i�ۑ��j

	// �E�B���h�E�̕\��
	ShowWindow(hWnd, SW_MAXIMIZE);
	UpdateWindow(hWnd);

	DWORD dwFrameCount;					// �t���[���J�E���g
	DWORD dwFPSLastTime;				// �Ō��FPS���v����������

	dwFrameCount = 0;
	dwFPSLastTime = timeGetTime();

	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// ImGui�̃C���X�^���X����
	CImGuiManager::Instance().Init(hWnd,pRenderer->GetDevice());

	//���b�Z�[�W���[�v
	while (1)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != 0)
		{// windows�̏���
			if (msg.message == WM_QUIT)
			{// WM_QUIT���b�Z�[�W���󂯎�����烁�b�Z�[�W���[�v�𔲂���
				break;
			}
			else
			{
				//���b�Z�[�W�̐ݒ�
				TranslateMessage(&msg);   // ���z���b�Z�[�W�𕶎����b�Z�[�W�֕ϊ�
				DispatchMessage(&msg);    // �E�B���h�E
			}
		}
		else
		{// DirectX�̏���

			dwCurrentTime = timeGetTime();// ���ݎ������擾

			if ((dwCurrentTime - dwFPSLastTime) >= 500)
			{// 0.5�b�o��

				// FPS���v��
				nCountFPS = (dwFrameCount * 1000) / (dwCurrentTime - dwFPSLastTime);

				pManager->GetFPS(nCountFPS);

				dwFPSLastTime = dwCurrentTime;// FPS���v������������ۑ�

				dwFrameCount = 0;// �t���[���J�E���g���N���A
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{// 60����1�b�o��

				dwExecLastTime = dwCurrentTime;// �����J�n�̎���[���ݎ���]��ۑ�

				dwCurrentTime = timeGetTime();// ���ݎ������擾

				dwFrameCount++;// �t���[���J�E���g�����Z

				if (pRenderer->NeedsReset())
				{
					// �f�o�C�X�̃��Z�b�g
					pRenderer->ResetDevice();
				}

				// �t���[���J�n
				CImGuiManager::Instance().NewFlameImgui();

				// �}�l�[�W���[�̍X�V����
				pManager->Update();

				ImGui::EndFrame();

				//�}�l�[�W���[�̕`�揈��
				pManager->Draw();
			}
		}
	}

	// �}�l�[�W���[�̔j��
	if (pManager != nullptr)
	{
		// �}�l�[�W���[�̏I������
		pManager->Uninit();

		delete pManager;
		pManager = nullptr;
	}

	// ����\��߂�
	timeEndPeriod(1);

	// �E�B���h�E�N���X�̓o�^������
	UnregisterClass(CLASS_NAME, wcex.hInstance);

	return (int)msg.wParam;
}

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

//=============================================================================
// �E�B���h�E�v���V�[�W��
//=============================================================================
LRESULT CALLBACK WindowProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	//HDC hDC;//�f�o�C�X�R���e�L�X�g(GDI�I�u�W�F�N�g)�̃n���h��

	if (ImGui_ImplWin32_WndProcHandler(hWnd, uMsg, wParam, lParam))
		return true;

	const RECT rect = { 0,0,SCREEN_WIDTH,SCREEN_HEIGHT };

	int nID;

	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// ���C���v���Z�X�ϐ�
	CMainProc pProc = {};

	switch (uMsg)
	{
	case WM_DESTROY:  //�E�B���h�E�j���̃��b�Z�[�W
		//WM_QUIT���b�Z�[�W�𑗂�
		PostQuitMessage(0);

		break;
	case WM_SIZE:

		if (wParam == SIZE_MINIMIZED)
		{
			return 0;
		}

		pRenderer->OnResize(LOWORD(lParam), HIWORD(lParam));

		return 0;

	case WM_KEYDOWN://�L�[�����̃��b�Z�[�W
		switch (wParam)
		{
		case VK_ESCAPE://[ESC]�L�[�������ꂽ

			//�E�B���h�E��j������
			nID = MessageBox(hWnd, "�I�����܂���?", "�I�����b�Z�[�W", MB_YESNO);
			if (nID == IDYES)
			{
				DestroyWindow(hWnd);//�E�C���h�E�j�����b�Z�[�W
			}
			else
			{
				return 0;            //�Ԃ�
			}

			break;

		case VK_F11:

			// �t���X�N���[��
			pProc.ToggleFullScreen(hWnd);

			break;
		}
		break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);//����̏������J��Ԃ�
}
//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMainProc::CMainProc()
{
	// �l�̃N���A
	m_Windowrect = {};
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMainProc::~CMainProc()
{
	// ����
}
//=============================================================================
// �E�B���h�E�t���X�N���[��
//=============================================================================
void CMainProc::ToggleFullScreen(HWND hWnd)
{
	// �؂�ւ��t���O��錾
	static bool isFullscreen = false;

	// ���݂̃E�B���h�E�X�^�C�����擾
	DWORD dwStyle = GetWindowLong(hWnd, GWL_STYLE);

	if (isFullscreen)
	{
		// �E�B���h�E���[�h�ɐ؂�ւ�
		SetWindowLong(hWnd, GWL_STYLE, dwStyle | WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, m_Windowrect.left, m_Windowrect.top,
			m_Windowrect.right - m_Windowrect.left, m_Windowrect.bottom - m_Windowrect.top,
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_NORMAL);
	}
	else
	{
		// �t���X�N���[�����[�h�ɐ؂�ւ�
		GetWindowRect(hWnd, &m_Windowrect);
		SetWindowLong(hWnd, GWL_STYLE, dwStyle & ~WS_OVERLAPPEDWINDOW);
		SetWindowPos(hWnd, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN),
			SWP_FRAMECHANGED | SWP_NOACTIVATE);
		ShowWindow(hWnd, SW_MAXIMIZE);
	}

	// �t���O��ύX
	isFullscreen = !isFullscreen;
}