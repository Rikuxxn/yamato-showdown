//=============================================================================
//
// �����_�����O���� [renderer.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "renderer.h"
#include "object.h"
#include "object2D.h"
#include "manager.h"
#include "game.h"
#include "imguimaneger.h"
#include "guiInfo.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CDebugProc* CRenderer::m_pDebug = nullptr;
CDebugProc3D* CRenderer::m_pDebug3D = nullptr;
int CRenderer::m_nFPS = 0;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CRenderer::CRenderer()
{
	// �l�̃N���A
	m_pD3D			= nullptr;
	m_pD3DDevice	= nullptr;
	m_ResizeWidth	= 0;
	m_ResizeHeight	= 0;
	m_d3dpp			= {};
	m_bgCol			= INIT_XCOL;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CRenderer::~CRenderer()
{
	// ���͂Ȃ�

}
//=============================================================================
// ����������
//=============================================================================
HRESULT CRenderer::Init(HWND hWnd, BOOL bWindow)
{
	D3DDISPLAYMODE d3ddm;			// �f�B�X�v���C���[�h

	// DirectX3D�I�u�W�F�N�g�̐���
	m_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (m_pD3D == nullptr)
	{
		return E_FAIL;
	}

	// ���݂̃f�B�X�v���C���[�h���擾
	if (FAILED(m_pD3D->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm)))
	{
		return E_FAIL;
	}

	// �f�o�C�X�̃v���[���e�[�V�����p�����[�^�̐ݒ�
	ZeroMemory(&m_d3dpp, sizeof(m_d3dpp));								// �p�����[�^�̃[���N���A

	m_d3dpp.BackBufferWidth				= SCREEN_WIDTH;					// �Q�[����ʃT�C�Y�i���j
	m_d3dpp.BackBufferHeight			= SCREEN_HEIGHT;				// �Q�[����ʃT�C�Y�i�����j
	m_d3dpp.BackBufferFormat			= d3ddm.Format;					// �o�b�N�o�b�t�@�̌`��
	m_d3dpp.BackBufferCount				= 1;							// �o�b�N�o�b�t�@�̐�
	m_d3dpp.SwapEffect					= D3DSWAPEFFECT_DISCARD;		// �_�u���o�b�t�@�̐؂�ւ�
	m_d3dpp.EnableAutoDepthStencil		= TRUE;							// �f�v�X�o�b�t�@�ƃX�e���V���o�b�t�@���쐬
	m_d3dpp.AutoDepthStencilFormat		= D3DFMT_D24S8;					// �f�v�X�o�b�t�@�Ƃ���16bit���g��
	m_d3dpp.Windowed					= bWindow;						// �E�C���h�E���[�h
	m_d3dpp.FullScreen_RefreshRateInHz	= D3DPRESENT_RATE_DEFAULT;		// ���t���b�V�����[�g
	m_d3dpp.PresentationInterval		= D3DPRESENT_INTERVAL_DEFAULT;	// �C���^�[�o��

	// DirectX3D�f�o�C�X�̐���
	if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
		D3DDEVTYPE_HAL,
		hWnd,
		D3DCREATE_HARDWARE_VERTEXPROCESSING,
		&m_d3dpp,
		&m_pD3DDevice)))
	{
		// DirectX3D�f�o�C�X�̐���
		if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
			D3DDEVTYPE_HAL,
			hWnd,
			D3DCREATE_SOFTWARE_VERTEXPROCESSING,
			&m_d3dpp,
			&m_pD3DDevice)))
		{
			// DirectX3D�f�o�C�X�̐���
			if (FAILED(m_pD3D->CreateDevice(D3DADAPTER_DEFAULT,
				D3DDEVTYPE_REF,
				hWnd,
				D3DCREATE_SOFTWARE_VERTEXPROCESSING,
				&m_d3dpp,
				&m_pD3DDevice)))
			{
				return E_FAIL;
			}
		}
	}

	// �����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �T���v���[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// �e�N�X�`���X�e�[�W�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// �f�o�b�O�t�H���g�̏�����
	m_pDebug->Init();

	// 3D�f�o�b�O�\���̏�����
	m_pDebug3D->Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CRenderer::Uninit(void)
{
	// �f�o�b�O�t�H���g�̏I������
	m_pDebug->Uninit();

	// 3D�f�o�b�O�\���̏I������
	m_pDebug3D->Uninit();

	// Direct3D�f�o�C�X�̔j��
	if (m_pD3DDevice != nullptr)
	{
		m_pD3DDevice->Release();
		m_pD3DDevice = nullptr;
	}

	// Direct3D�I�u�W�F�N�g�̔j��
	if (m_pD3D != nullptr)
	{
		m_pD3D->Release();
		m_pD3D = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CRenderer::Update(void)
{
	// ���ׂẴI�u�W�F�N�g�̍X�V����
	CObject::UpdateAll();

#ifdef _DEBUG

	// �ꏊ
	CImGuiManager::Instance().SetPosImgui(ImVec2(20.0f, 20.0f));

	// �T�C�Y
	CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 500.0f));

	// �ŏ���Gui
	CImGuiManager::Instance().StartImgui(u8"DebugInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// FPS�l�̎擾
	int fps = GetFPS();

	// FPS�l
	ImGui::Text("FPS : %d", fps);

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �V�[�h�l
	ImGui::Text("SEED : %d", CGame::GetSeed());

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	ImGui::Text("BG Color:");

	ImGui::ColorEdit4("col", m_bgCol);

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �v���C���[�̃f�o�b�O���̕\������
	CGuiInfo::PlayerInfo();

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �G�̃f�o�b�O���̕\������
	CGuiInfo::EnemyInfo();

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �J�����̃f�o�b�O���̕\������
	CGuiInfo::CameraInfo();

	ImGui::End();

#endif

}
//=============================================================================
// �`�揈��
//=============================================================================
void CRenderer::Draw(int fps)
{
	// ��ʃN���A
	m_pD3DDevice->Clear(0, NULL,
		(D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL),
		m_bgCol, 1.0f, 0);

	// �`��J�n
	if (SUCCEEDED(m_pD3DDevice->BeginScene()))
	{// �`��J�n�����������ꍇ

		// ���ׂẴI�u�W�F�N�g�̕`�揈��
		CObject::DrawAll();

		// �t�F�[�h�̎擾
		CFade* pFade = CManager::GetFade();

		// �t�F�[�h�̕`�揈��
		if (pFade != nullptr)
		{
			// �t�F�[�h�̕`�揈��
			pFade->Draw();
		}

		// FPS�̃Z�b�g
		SetFPS(fps);
		
#ifdef _DEBUG

		// ImGui�̕`��I��
		CImGuiManager::Instance().EndImguiRender();
#endif
		// �`��I��
		m_pD3DDevice->EndScene();
	}

	// �o�b�N�o�b�t�@�ƃt�����g�o�b�t�@�̓���ւ�
	m_pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
//=============================================================================
// �f�o�C�X�̃��Z�b�g
//=============================================================================
void CRenderer::ResetDevice(void)
{
	if (!m_pD3DDevice || m_ResizeWidth == 0 || m_ResizeHeight == 0)
	{
		return;
	}

	// �f�o�b�O�t�H���g�̔j��
	m_pDebug->Uninit();

	// 3D�f�o�b�O�\���̔j��
	m_pDebug3D->Uninit();

	m_d3dpp.BackBufferWidth = m_ResizeWidth;
	m_d3dpp.BackBufferHeight = m_ResizeHeight;
	m_ResizeWidth = m_ResizeHeight = 0;

	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT hr = m_pD3DDevice->Reset(&m_d3dpp);

	if (hr == D3DERR_INVALIDCALL)
	{
		IM_ASSERT(0);
	}

	ImGui_ImplDX9_CreateDeviceObjects();

	// �����_�[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	m_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	m_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	m_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// �T���v���[�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	m_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	// �e�N�X�`���X�e�[�W�X�e�[�g�̐ݒ�
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	m_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);

	// �f�o�b�O�t�H���g�̏�����
	m_pDebug->Init();

	// 3D�f�o�b�O�\���̏�����
	m_pDebug3D->Init();

	CLight* pLight = CManager::GetLight();

	// ���C�g�̏���������
	pLight->Init();

	// ���C�g�̏����ݒ菈��
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 300.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
}
//=============================================================================
// ���_�V�F�[�_�R���p�C���p�֐�
//=============================================================================
HRESULT CRenderer::CompileVertexShader(
	const char* filename,         // HLSL�t�@�C����
	const char* entryPoint,       // �G���g���|�C���g��
	LPDIRECT3DVERTEXSHADER9* ppVS,// �쐬�����V�F�[�_�̃|�C���^
	LPD3DXCONSTANTTABLE* ppConsts // �萔�e�[�u�����󂯎��ꍇ
)
{
	if (!m_pD3DDevice)
	{
		return E_FAIL;
	}

	LPD3DXBUFFER pCode = nullptr;
	LPD3DXBUFFER pErr = nullptr;

	HRESULT hr = D3DXCompileShaderFromFile(
		filename,
		nullptr,
		nullptr,
		entryPoint,
		"vs_3_0",
		D3DXSHADER_DEBUG,
		&pCode,
		&pErr,
		ppConsts
	);

	if (FAILED(hr))
	{
		if (pErr)
		{
			OutputDebugStringA((char*)pErr->GetBufferPointer());
			pErr->Release();
		}
		if (pCode)
		{
			pCode->Release();
		}

		return hr;
	}

	hr = m_pD3DDevice->CreateVertexShader(
		(DWORD*)pCode->GetBufferPointer(),
		ppVS
	);

	pCode->Release();
	if (pErr)
	{
		pErr->Release();
	}

	return hr;
}
//=============================================================================
// �s�N�Z���V�F�[�_�R���p�C���p�֐�
//=============================================================================
HRESULT CRenderer::CompilePixelShader(
	const char* filename,         // HLSL�t�@�C����
	const char* entryPoint,       // �G���g���|�C���g��
	LPDIRECT3DPIXELSHADER9* ppPS, // �쐬�����V�F�[�_�̃|�C���^
	LPD3DXCONSTANTTABLE* ppConsts // �萔�e�[�u�����󂯎��ꍇ
)
{
	if (!m_pD3DDevice)
	{
		return E_FAIL;
	}

	LPD3DXBUFFER pCode = nullptr;
	LPD3DXBUFFER pErr = nullptr;

	HRESULT hr = D3DXCompileShaderFromFile(
		filename,
		nullptr,
		nullptr,
		entryPoint,
		"ps_3_0",
		D3DXSHADER_DEBUG,
		&pCode,
		&pErr,
		ppConsts
	);

	if (FAILED(hr))
	{
		if (pErr)
		{
			OutputDebugStringA((char*)pErr->GetBufferPointer());
			pErr->Release();
		}
		if (pCode)
		{
			pCode->Release();
		}

		return hr;
	}

	hr = m_pD3DDevice->CreatePixelShader(
		(DWORD*)pCode->GetBufferPointer(),
		ppPS
	);

	pCode->Release();
	if (pErr)
	{
		pErr->Release();
	}

	return hr;
}
//=============================================================================
// �T�C�Y�̍Đݒ�
//=============================================================================
void CRenderer::OnResize(UINT width, UINT height)
{
	m_ResizeWidth = width;
	m_ResizeHeight = height;
}
//=============================================================================
// �f�o�C�X�̃��Z�b�g���K�v���ǂ���
//=============================================================================
bool CRenderer::NeedsReset() const
{
	return (m_ResizeWidth != 0 && m_ResizeHeight != 0);
}