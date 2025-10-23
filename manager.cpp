//=============================================================================
//
// �}�l�[�W���[���� [manager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "manager.h"
#include "renderer.h"
#include "sound.h"
#include "game.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CRenderer* CManager::m_pRenderer = nullptr;
CInputKeyboard* CManager::m_pInputKeyboard = nullptr;
CInputJoypad* CManager::m_pInputJoypad = nullptr;
CInputMouse* CManager::m_pInputMouse = nullptr;
CSound* CManager::m_pSound = nullptr;
CTexture* CManager::m_pTexture = nullptr;
CCamera* CManager::m_pCamera = nullptr;
CLight* CManager::m_pLight = nullptr;
CScene* CManager::m_pScene = nullptr;
CFade* CManager::m_pFade = nullptr;
std::unique_ptr<btDiscreteDynamicsWorld> CManager::m_pDynamicsWorld = nullptr;

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CManager::CManager()
{
	// �l�̃N���A
	m_fps						= 0;		// FPS�l
	m_pBroadphase				= nullptr;	// �Փ˔���̃N���X�ւ̃|�C���^
	m_pCollisionConfiguration	= nullptr;	// �Փˌ��o�̐ݒ���Ǘ�����N���X�ւ̃|�C���^
	m_pDispatcher				= nullptr;	// ���ۂɏՓ˔��菈�������s����N���X�ւ̃|�C���^
	m_pSolver					= nullptr;	// �����V�~�����[�V�����̐���\���o�[�ւ̃|�C���^
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CManager::~CManager()
{

}
//=============================================================================
// ����������
//=============================================================================
HRESULT CManager::Init(HINSTANCE hInstance, HWND hWnd)
{
	// �����_���[�̐���
	m_pRenderer = new CRenderer;

	// �����_���[�̏���������
	if (FAILED(m_pRenderer->Init(hWnd, TRUE)))
	{
		return -1;
	}

	// �L�[�{�[�h�̐���
	m_pInputKeyboard = new CInputKeyboard;

	// �L�[�{�[�h�̏���������
	if (FAILED(m_pInputKeyboard->Init(hInstance, hWnd)))
	{
		return -1;
	}

	// �W���C�p�b�h�̐���
	m_pInputJoypad = new CInputJoypad;

	// �W���C�p�b�h�̏���������
	if (FAILED(m_pInputJoypad->Init()))
	{
		return E_FAIL;
	}

	// �}�E�X�̐���
	m_pInputMouse = new CInputMouse;

	// �}�E�X�̏���������
	if (FAILED(m_pInputMouse->Init(hInstance, hWnd)))
	{
		return E_FAIL;
	}

	// �T�E���h�̐���
	m_pSound = new CSound;

	// �T�E���h�̏���������
	if (FAILED(m_pSound->Init(hWnd)))
	{
		return E_FAIL;
	}

	// Bullet�������[���h�̐���
	m_pBroadphase = make_unique<btDbvtBroadphase>();
	m_pCollisionConfiguration = make_unique <btDefaultCollisionConfiguration>();
	m_pDispatcher = make_unique <btCollisionDispatcher>(m_pCollisionConfiguration.get());
	m_pSolver = make_unique <btSequentialImpulseConstraintSolver>();

	m_pDynamicsWorld = make_unique <btDiscreteDynamicsWorld>(m_pDispatcher.get(), m_pBroadphase.get(), m_pSolver.get(), m_pCollisionConfiguration.get());

	// �d�͂�ݒ�
	m_pDynamicsWorld->setGravity(btVector3(0, -630.0f, 0));

	// �J�����̐���
	m_pCamera = new CCamera;

	// �J�����̏���������
	m_pCamera->Init();

	// ���C�g�̐���
	m_pLight = new CLight;

	// ���C�g�̏���������
	m_pLight->Init();

	// �e�N�X�`���̐���
	m_pTexture = new CTexture;

	// �e�N�X�`���̓ǂݍ���
	m_pTexture->Load();

	// �^�C�g�����
	m_pFade = CFade::Create(CScene::MODE_TITLE);

	// �^�C�g�����
	m_pScene = CScene::Create(CScene::MODE_TITLE);

	// �^�C�g����ʂ�������
	if (m_pScene->GetMode() == MODE_TITLE)
	{// �J�����̈ʒu�̐ݒ�
		D3DXVECTOR3 posV(D3DXVECTOR3(-1373.5f, 331.5f, -1130.5f));
		D3DXVECTOR3 posR(D3DXVECTOR3(-913.5f, 150.0f, -1700.0f));

		m_pCamera->SetPosV(posV);
		m_pCamera->SetPosR(posR);
		m_pCamera->SetRot(D3DXVECTOR3(0.25f, -0.70f, 0.0f));
		m_pCamera->SetDis(sqrtf(
			((posV.x - posR.x) * (posV.x - posR.x)) +
			((posV.y - posR.y) * (posV.y - posR.y)) +
			((posV.z - posR.z) * (posV.z - posR.z))));
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CManager::Uninit(void)
{
	// ���ׂẴI�u�W�F�N�g�̔j��
	CObject::ReleaseAll();

	// �e�N�X�`���̔j��
	if (m_pTexture != nullptr)
	{
		// �S�Ẵe�N�X�`���̔j��
		m_pTexture->Unload();

		delete m_pTexture;
		m_pTexture = nullptr;
	}

	// �L�[�{�[�h�̏I������
	m_pInputKeyboard->Uninit();

	// �W���C�p�b�h�̏I������
	m_pInputJoypad->Uninit();

	// �}�E�X�̏I������
	m_pInputMouse->Uninit();

	// �T�E���h�̏I������
	m_pSound->Uninit();

	// �L�[�{�[�h�̔j��
	if (m_pInputKeyboard != nullptr)
	{
		// �����_���[�̏I������
		m_pInputKeyboard->Uninit();

		delete m_pInputKeyboard;
		m_pInputKeyboard = nullptr;
	}

	// �W���C�p�b�h�̔j��
	if (m_pInputJoypad != nullptr)
	{
		// �W���C�p�b�h�̏I������
		m_pInputJoypad->Uninit();

		delete m_pInputJoypad;
		m_pInputJoypad = nullptr;
	}

	// �}�E�X�̔j��
	if (m_pInputMouse != nullptr)
	{
		// �}�E�X�̏I������
		m_pInputMouse->Uninit();

		delete m_pInputMouse;
		m_pInputMouse = nullptr;
	}

	// �T�E���h�̔j��
	if (m_pSound != nullptr)
	{
		// �}�E�X�̏I������
		m_pSound->Uninit();

		delete m_pSound;
		m_pSound = nullptr;
	}

	// �J�����̔j��
	if (m_pCamera != nullptr)
	{
		delete m_pCamera;
		m_pCamera = nullptr;
	}

	// ���C�g�̔j��
	if (m_pLight != nullptr)
	{
		delete m_pLight;
		m_pLight = nullptr;
	}

	// �t�F�[�h�̔j��
	if (m_pFade != nullptr)
	{
		// �t�F�[�h�̏I������
		m_pFade->Uninit();

		delete m_pFade;
		m_pFade = nullptr;
	}

	// �����_���[�̔j��
	if (m_pRenderer != nullptr)
	{
		// �����_���[�̏I������
		m_pRenderer->Uninit();

		delete m_pRenderer;
		m_pRenderer = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CManager::Update(void)
{
	// �L�[�{�[�h�̍X�V
	m_pInputKeyboard->Update();

	// �W���C�p�b�h�̍X�V
	m_pInputJoypad->Update();

	// �}�E�X�̍X�V
	m_pInputMouse->Update();

	// �t�F�[�h�̍X�V����
	if (m_pFade != nullptr)
	{
		// �t�F�[�h�̍X�V����
		m_pFade->Update();
	}

	// �|�[�Y���̓Q�[���X�V���Ȃ�
	if (CGame::GetisPaused() == true)
	{
		if (m_pInputKeyboard->GetTrigger(DIK_TAB) || m_pInputJoypad->GetTrigger(CInputJoypad::JOYKEY_START))
		{
			CGame::SetEnablePause(false);
		}

		// �}�E�X�J�[�\����\���ɂ���
		m_pInputMouse->SetCursorVisibility(true);

		// �|�[�Y�}�l�[�W���[�̍X�V����
		CGame::GetPauseManager()->Update();

		return;
	}

	m_pDynamicsWorld->stepSimulation((btScalar)m_fps);

	// �J�����̍X�V
	m_pCamera->Update();

	// ���C�g�̍X�V
	m_pLight->Update();

	// �����_���[�̍X�V
	m_pRenderer->Update();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CManager::Draw(void)
{
	// �����_���[�̕`��
	m_pRenderer->Draw(m_fps);
}
//=============================================================================
// ���[�h�̐ݒ�
//=============================================================================
void CManager::SetMode(CScene::MODE mode)
{
	// �J�����̏���������
	m_pCamera->Init();

	// �T�E���h�̒�~
	m_pSound->Stop();

	if (m_pScene != nullptr)
	{
		// ���݂̃��[�h�j��
		m_pScene->Uninit();
	}

	// �S�ẴI�u�W�F�N�g��j��
	CObject::ReleaseAll();

	// �|�[�Y��false�ɂ��Ă���
	CGame::SetEnablePause(false);

	// �V�������[�h�̐���
	m_pScene = CScene::Create(mode);
}
//=============================================================================
// ���݂̃��[�h�̎擾
//=============================================================================
CScene::MODE CManager::GetMode(void)
{
	return m_pScene->GetMode();
}