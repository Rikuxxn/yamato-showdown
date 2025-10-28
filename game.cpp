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
#include "charactermanager.h"
#include "player.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CPlayer* CGame::m_pPlayer = nullptr;				// �v���C���[�ւ̃|�C���^
CEnemy* CGame::m_pEnemy = nullptr;					// �G�ւ̃|�C���^
CTime* CGame::m_pTime = nullptr;					// �^�C���ւ̃|�C���^
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
	m_pRankingManager = nullptr;
	m_pLight = nullptr;
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

	// ���C�g�̐���
	m_pLight = new CLight;

	// ���C�g�̏�����
	m_pLight->Init();

	// ���C�g�̍Đݒ菈��
	ResetLight();

	CCharacterManager characterManager;

	// �v���C���[�̐���
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	characterManager.AddCharacter(m_pPlayer);

	// �G�̐���
	m_pEnemy = CEnemy::Create(D3DXVECTOR3(0.0f, 200.0f, 300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	characterManager.AddCharacter(m_pEnemy);

	m_nSeed = (int)time(nullptr);  // �V�[�h�l�������_���ݒ�

	// �}�b�v�����_������
	m_pBlockManager->GenerateRandomMap(m_nSeed);

	// �^�C���̐���
	m_pTime = CTime::Create(3, 0, 760.0f, 10.0f, 42.0f, 58.0f);

	//// �|�[�YUI�̐���
	//m_pUi = CUi::Create<CPauseUi>("data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 855.0f, 0.0f), 160.0f, 35.0f);

	// �|�[�Y�}�l�[�W���[�̐���
	m_pPauseManager = new CPauseManager();

	// �|�[�Y�}�l�[�W���[�̏�����
	m_pPauseManager->Init();

	// �����L���O�}�l�[�W���[�̃C���X�^���X����
	m_pRankingManager = make_unique<CRankingManager>();

	// �t���O�̃��Z�b�g
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

	// ���C�g�̔j��
	if (m_pLight != nullptr)
	{
		delete m_pLight;
		m_pLight = nullptr;
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
			{// ����������ɓ��ꂽ��
				CResult::SetGet(true);
			}
		}
	}

	// �G��|������N���A
	if (pFade->GetFade() == CFade::FADE_NONE && CGame::GetEnemy()->IsDead())
	{
		// �����L���O�ɓo�^
		m_pRankingManager->AddRecordWithLimit(3, 0, m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// ���U���g�ɃZ�b�g
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// ���U���g��ʂɈڍs
		pFade->SetFade(MODE_RESULT);
	}

#ifdef _DEBUG
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pFade->GetFade() == CFade::FADE_NONE && pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// �����L���O�ɓo�^
		m_pRankingManager->AddRecordWithLimit(3, 0, m_pTime->GetMinutes(), m_pTime->GetnSeconds());

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
// ���C�g�̍Đݒ菈��
//=============================================================================
void CGame::ResetLight(void)
{
	// ���C�g���폜���Ă���
	CLight::Uninit();

	// ���C�����C�g�i�[���j: �I�����W�{�΂߂���Ǝ�
	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		D3DXCOLOR(1.0f, 0.6f, 0.3f, 1.0f),   // �g�����I�����W
		D3DXVECTOR3(0.5f, -1.0f, 0.3f),      // �E�と�����ɍ���
		D3DXVECTOR3(0.0f, 300.0f, 0.0f)
	);

	// �T�u���C�g�i��̔��ˌ��j: �ア�F�A�ォ��
	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		D3DXCOLOR(0.3f, 0.4f, 0.8f, 1.0f),   // ������
		D3DXVECTOR3(0.0f, -1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f)
	);

	// �����ق�̂�Ԃ݂ŕ�ށi�t���⏕�j
	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		D3DXCOLOR(0.7f, 0.3f, 0.2f, 1.0f),
		D3DXVECTOR3(-0.3f, 0.0f, -0.7f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f)
	);

	// �c��͕⏕�I�ȒW�����i��┒���j
	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		D3DXCOLOR(0.4f, 0.35f, 0.3f, 1.0f),
		D3DXVECTOR3(0.0f, 0.0f, 1.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f)
	);

	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		D3DXCOLOR(0.3f, 0.25f, 0.2f, 1.0f),
		D3DXVECTOR3(0.0f, 0.0f, -1.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f)
	);
}
//=============================================================================
// �f�o�C�X���Z�b�g�ʒm
//=============================================================================
void CGame::OnDeviceReset(void)
{
	// ���C�g�̍Đݒ菈��
	ResetLight();
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
