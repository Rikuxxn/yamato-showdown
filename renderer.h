//=============================================================================
//
// �����_�����O���� [renderer.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RENDERER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RENDERER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "debugproc.h"
#include "debugproc3D.h"
#include "imguimaneger.h"

//*****************************************************************************
// �����_���[�N���X
//*****************************************************************************
class CRenderer
{
public:
	CRenderer();
	~CRenderer();

	HRESULT Init(HWND hWnd, BOOL bWindow);
	void Uninit(void);
	void Update(void);
	void Draw(int fps);
	void ResetDevice(void);
	void OnResize(UINT width, UINT height);

	HRESULT CompileVertexShader(
		const char* filename,         // HLSL�t�@�C����
		const char* entryPoint,       // �G���g���|�C���g��
		LPDIRECT3DVERTEXSHADER9* ppVS,// �쐬�����V�F�[�_�̃|�C���^
		LPD3DXCONSTANTTABLE* ppConsts // �萔�e�[�u�����󂯎��ꍇ
	);
	HRESULT CompilePixelShader(
		const char* filename,         // HLSL�t�@�C����
		const char* entryPoint,       // �G���g���|�C���g��
		LPDIRECT3DPIXELSHADER9* ppPS, // �쐬�����V�F�[�_�̃|�C���^
		LPD3DXCONSTANTTABLE* ppConsts // �萔�e�[�u�����󂯎��ꍇ
	);

	//*****************************************************************************
	// flagment�֐�
	//*****************************************************************************
	bool NeedsReset(void) const;

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetFPS(int fps) { m_nFPS = fps; }
	void SetBgCol(D3DXCOLOR col) { m_bgCol = col; }

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	static CDebugProc* GetDebug(void) { return m_pDebug; }
	static CDebugProc3D* GetDebug3D(void) { return m_pDebug3D; }
	static int GetFPS(void) { return m_nFPS; }
	LPDIRECT3DDEVICE9 GetDevice(void) { return m_pD3DDevice; };
	D3DXCOLOR GetBgCol(void) { return m_bgCol; }

private:
	LPDIRECT3D9 m_pD3D;						// DirectX3D�I�u�W�F�N�g�ւ̃|�C���^
	LPDIRECT3DDEVICE9 m_pD3DDevice;			// �f�o�C�X�ւ̃|�C���^
	static CDebugProc* m_pDebug;			// �f�o�b�O�t�H���g�ւ̃|�C���^
	static CDebugProc3D* m_pDebug3D;		// 3D�f�o�b�O�\���ւ̃|�C���^
	D3DXCOLOR m_bgCol;						// ��ʔw�i�̐F
	UINT m_ResizeWidth;						// �Đݒ�p�̉�ʂ̕�
	UINT m_ResizeHeight;					// �Đݒ�p�̉�ʂ̍���
	D3DPRESENT_PARAMETERS m_d3dpp;			// �Đݒ�p�̃p�����[�^�[
	static int m_nFPS;						// FPS�l�̑���p
};
#endif