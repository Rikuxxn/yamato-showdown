//=============================================================================
//
// �Q�[������ [game.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "game.h"
#include "manager.h"
#include "result.h"
#include "particle.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CPlayer* CGame::m_pPlayer = nullptr;
CTime* CGame::m_pTime = nullptr;					// �^�C���ւ̃|�C���^
CColon* CGame::m_pColon = nullptr;					// �R�����ւ̃|�C���^
CBlock* CGame::m_pBlock= nullptr;					// �u���b�N�ւ̃|�C���^
CBlockManager* CGame::m_pBlockManager= nullptr;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^
CObjectBillboard* CGame::m_pBillboard = nullptr;	// �r���{�[�h�ւ̃|�C���^
CPauseManager* CGame::m_pPauseManager = nullptr;	// �|�[�Y�}�l�[�W���[�ւ̃|�C���^
bool CGame::m_isPaused = false;						// true�Ȃ�|�[�Y��
int CGame::m_nSeed = 0;								// �}�b�v�̃V�[�h�l

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGame::CGame() : CScene(CScene::MODE_GAME)
{
	// �l�̃N���A
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CGame::~CGame()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CGame::Init(void)
{
#ifdef _DEBUG
	// �O���b�h�̐���
	m_pGrid = CGrid::Create();

	// �O���b�h�̏�����
	m_pGrid->Init();
#endif

	// �u���b�N�}�l�[�W���[�̐���
	m_pBlockManager = new CBlockManager;

	// �u���b�N�}�l�[�W���[�̏�����
	m_pBlockManager->Init();

	// �v���C���[�̐���
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	m_nSeed = (int)time(nullptr);  // �V�[�h�l�������_���ݒ�

	// �}�b�v�����_������
	m_pBlockManager->GenerateRandomMap(m_nSeed);

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// �^�C���̐���
	m_pTime = CTime::Create(0, 0, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// �R�����̐���
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	//// �|�[�YUI�̐���
	//m_pUi = CUi::Create<CPauseUi>("data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 855.0f, 0.0f), 160.0f, 35.0f);

	// �|�[�Y�}�l�[�W���[�̐���
	m_pPauseManager = new CPauseManager();

	// �|�[�Y�}�l�[�W���[�̏�����
	m_pPauseManager->Init();

	CResult::SetGet(false);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CGame::Uninit(void)
{
	// ���̒�~
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_GAMEBGM);

	// �u���b�N�}�l�[�W���[�̔j��
	if (m_pBlockManager != nullptr)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = nullptr;
	}

	// �|�[�Y�}�l�[�W���[�̔j��
	if (m_pPauseManager != nullptr)
	{
		// �|�[�Y�}�l�[�W���[�̏I������
		m_pPauseManager->Uninit();

		delete m_pPauseManager;
		m_pPauseManager = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CGame::Update(void)
{
	CFade* pFade = CManager::GetFade();
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// TAB�L�[�Ń|�[�YON/OFF
	if (pKeyboard->GetTrigger(DIK_TAB) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_START))
	{
		// �|�[�YSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_PAUSE);

		// �|�[�Y�؂�ւ��O�̏�Ԃ��L�^
		bool wasPaused = m_isPaused;

		m_isPaused = !m_isPaused;

		// �|�[�Y��Ԃɉ����ĉ��𐧌�
		if (m_isPaused && !wasPaused)
		{
			// �ꎞ��~����
			CManager::GetSound()->PauseAll();
		}
		else if (!m_isPaused && wasPaused)
		{
			// �ĊJ����
			CManager::GetSound()->ResumeAll();
		}
	}

	// �u���b�N�}�l�[�W���[�̍X�V����
	m_pBlockManager->Update();

	// �I������`�F�b�N
	if (m_pBlockManager)
	{
		for (auto block : m_pBlockManager->GetAllBlocks())
		{
			if (block->IsGet())
			{// �B���ꂽ������ɓ��ꂽ��
				CResult::SetGet(true);
			}

			if (block->IsEnd())
			{
				// ���U���g�ɃZ�b�g
				CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

				// ���U���g��ʂɈڍs
				pFade->SetFade(MODE_RESULT);

				break;
			}
		}
	}

#ifdef _DEBUG
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pFade->GetFade() == CFade::FADE_NONE && pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// ���U���g�ɃZ�b�g
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// ���U���g��ʂɈڍs
		pFade->SetFade(MODE_RESULT);
	}
#endif
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGame::Draw(void)
{
#ifdef _DEBUG
	// �O���b�h�̕`��
	m_pGrid->Draw();
#endif
	// �|�[�Y����������
	if (m_isPaused)
	{
		// �|�[�Y�}�l�[�W���[�̕`�揈��
		m_pPauseManager->Draw();
	}
}
//=============================================================================
// �|�[�Y�̐ݒ�
//=============================================================================
void CGame::SetEnablePause(bool bPause)
{
	m_isPaused = bPause;

	if (bPause)
	{
		// �����ꎞ��~
		CManager::GetSound()->PauseAll();
	}
	else
	{
		// �����ĊJ
		CManager::GetSound()->ResumeAll();
	}
}
