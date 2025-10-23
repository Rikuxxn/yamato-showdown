//=============================================================
//
//�@Imguimaneger[Imguimaneger.cpp]
//�@Author : TANEKAWA RIKU
//
//=============================================================

//*************************************************************
//�C���N���[�h�t�@�C��
//*************************************************************
#include "imguimaneger.h"
#include "imgui_internal.h"

// ���O��Ԃ̎g�p
using namespace std;

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::unique_ptr<CImGuiManager> CImGuiManager::m_Instance = nullptr;

//=============================================================
// �R���X�g���N�^
//=============================================================
CImGuiManager::CImGuiManager()
{
	// �l�̃N���A
}
//=============================================================
// �f�X�g���N�^
//=============================================================
CImGuiManager::~CImGuiManager()
{
	Uninit();
}
//=============================================================
// ImGui�̃C���X�^���X��������
//=============================================================
CImGuiManager& CImGuiManager::Instance(void)
{
	if (!m_Instance) // �܂�����Ă��Ȃ���΍��
	{
		m_Instance = std::make_unique<CImGuiManager>();
	}
	return *m_Instance;
}
//=============================================================
// ImGui�̏���������
//=============================================================
void CImGuiManager::Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice)
{

	// Setup Dear ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// Setup Dear ImGui style
	ImGui::StyleColorsDark();
	// ImGui::StyleColorsLight(); // ���̃X�^�C�����I�ׂ܂�

	// �t�H���g�ݒ�i�f�t�H���g�̃t�H���g�Ƃ��� Consolas ���g�p�j
	ImFont* myFont = io.Fonts->AddFontFromFileTTF("C:/Windows/Fonts/meiryo.ttc", 23.0f,NULL,io.Fonts->GetGlyphRangesJapanese());

	// Setup Platform/Renderer backends
	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX9_Init(pDevice);
}

//=============================================================
// ImGui�̏I������
//=============================================================
void CImGuiManager::Uninit()
{
	// Cleanup
	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
//=============================================================
// ImGui�̃E�B���h�E�̈ʒu��ݒ�
//=============================================================
void CImGuiManager::SetPosImgui(ImVec2 Pos)
{
	ImGui::SetNextWindowPos(Pos, ImGuiCond_Once);
}
//=============================================================
// ImGui�̃E�B���h�E�̑傫����ݒ�
//=============================================================
void CImGuiManager::SetSizeImgui(ImVec2 Size)
{
	ImGui::SetNextWindowSize(Size, ImGuiCond_Once);
}
//=============================================================
// ImGui�̃E�B���h�E�̕`��J�n
//=============================================================
void CImGuiManager::StartImgui(const char* ImguiName, IMGUITYPE nType)
{
	if (ImguiName != NULL)
	{
		switch (nType)
		{
		case IMGUITYPE_DEFOULT:
			ImGui::Begin(ImguiName);
			break;
		case IMGUITYPE_NOMOVE:
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove);
			break;
		case IMGUITYPE_NOMOVESIZE:
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoResize);
			break;
		case IMGUITYPE_NOMOVEANDSIZE:
			ImGui::Begin(ImguiName, NULL, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			break;
		default:
			break;
		}
	}
}
//=============================================================
// ImGui�̃E�B���h�E�̕`����I��
//=============================================================
void CImGuiManager::EndImgui()
{
	ImGui::End();
}
//=============================================================
// ImGui�̃E�B���h�E�̃t���[����ݒ�
//=============================================================
void CImGuiManager::NewFlameImgui()
{
	// Start the Dear ImGui frame
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}
//=============================================================
// ImGui�̃E�B���h�E�̃����_�[��ݒ�
//=============================================================
void CImGuiManager::EndImguiRender()
{
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}
