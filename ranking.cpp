//=============================================================================
//
// �����L���O���� [ranking.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "ranking.h"
#include "manager.h"
#include "input.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CRanking::CRanking() : CScene(CScene::MODE_RANKING)
{
	// �l�̃N���A
	m_pRankingManager = nullptr;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CRanking::~CRanking()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CRanking::Init(void)
{
	// �����L���O�}�l�[�W���[�̃C���X�^���X����
	m_pRankingManager = CRankingManager::GetInstance();

	// �����L���O�^�C���̐���
	m_pRankTime = CRankTime::Create(600.0f, 170.0f, 62.0f, 78.0f);

	// �����L���O�f�[�^���Z�b�g
	if (m_pRankingManager)
	{
		std::vector<std::pair<int, int>> rankList;

		for (auto& r : m_pRankingManager->GetList())
		{
			rankList.push_back({ r.minutes, r.seconds });
		}

		// �^�C�����X�g�̐ݒ�
		m_pRankTime->SetRankList(rankList);
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CRanking::Uninit(void)
{
	// �����L���O�}�l�[�W���[�̔j��
	if (m_pRankingManager != nullptr)
	{
		delete m_pRankingManager;
		m_pRankingManager = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CRanking::Update(void)
{
	// ���͎�t�̂��߂ɃC���v�b�g�������擾
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// ��ʑJ�ڂ̂��߂Ƀt�F�[�h���擾
	CFade* pFade = CManager::GetFade();

	// ��ʑJ��
	if (pFade->GetFade() == CFade::FADE_NONE && (pKeyboard->GetAnyKeyTrigger() || pJoypad->GetTrigger(pJoypad->JOYKEY_A)))
	{
		// �^�C�g����ʂɈڍs
		CManager::GetFade()->SetFade(CScene::MODE_TITLE);
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CRanking::Draw(void)
{




}