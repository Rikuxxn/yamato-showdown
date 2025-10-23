//=============================================================
//
// ImGui�̐��䏈�� [ImguiManager.h]
// Author : TANEKAWA RIKU
//
//=============================================================
#ifndef _IMGUIMANEGER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _IMGUIMANEGER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "imgui.h"
#include "imgui_impl_dx9.h"
#include "imgui_impl_win32.h"


//*****************************************************************************
// ImGui�}�l�[�W���[�N���X
//*****************************************************************************
class CImGuiManager
{
public :
	CImGuiManager();
	~CImGuiManager();

	typedef enum
	{
		IMGUITYPE_DEFOULT = 0,
		IMGUITYPE_NOMOVE,
		IMGUITYPE_NOMOVESIZE,
		IMGUITYPE_NOMOVEANDSIZE,
		IMGUITYPE_MAX
	}IMGUITYPE;

	static CImGuiManager& Instance(void);
	void Init(HWND hWnd, LPDIRECT3DDEVICE9 pDevice);
	void Uninit();
	void SetPosImgui(ImVec2 Pos);
	void SetSizeImgui(ImVec2 Size);
	void StartImgui(const char* ImguiName, IMGUITYPE nType);
	void EndImgui();
	void EndImguiRender();
	void NewFlameImgui();

private:
	static std::unique_ptr<CImGuiManager> m_Instance;
};

#endif // !_IMGUIMANEGER_H_