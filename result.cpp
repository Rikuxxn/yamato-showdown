//=============================================================================
//
// ���U���g���� [result.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "result.h"
#include "input.h"
#include "manager.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
CTime* CResult::m_pTime = nullptr;					// �^�C���ւ̃|�C���^
int CResult::m_nClearMinutes = 0;
int CResult::m_nClearSeconds = 0;
bool CResult::m_isTreasureGet = false;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CResult::CResult() : CScene(CScene::MODE_RESULT)
{
	// �l�̃N���A
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CResult::~CResult()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CResult::Init(void)
{
	// �}�E�X�J�[�\����\������
	CManager::GetInputMouse()->SetCursorVisibility(true);

	//// �w�i�̐���
	//CBackground::Create(D3DXVECTOR3(960.0f, 540.0f, 0.0f), 960.0f, 540.0f, "data/TEXTURE/resultBG.png");

	// ���v�b�ň���
	int totalLimit = 3 * 60 + 0;
	int totalRemain = m_nClearMinutes * 60 + m_nClearSeconds;

	// �N���A�^�C���i�o�ߎ��ԁj���v�Z
	int totalClear = totalLimit - totalRemain;
	if (totalClear < 0)
	{
		totalClear = 0; // �O�̂���
	}

	// ���ƕb�ɖ߂�
	int clearMinutes = totalClear / 60;
	int clearSeconds = totalClear % 60;

	// �^�C���̐���
	m_pTime = CTime::Create(clearMinutes, clearSeconds, 1100.0f, 695.0f, 72.0f, 88.0f);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CResult::Uninit(void)
{
	SetGet(false);
}
//=============================================================================
// �X�V����
//=============================================================================
void CResult::Update(void)
{
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();

	if (pFade->GetFade() == CFade::FADE_NONE && (pInputMouse->GetTrigger(0) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A)))
	{
		// �����L���O��ʂɈڍs
		pFade->SetFade(MODE_RANKING);
	}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN))
	{
		// �����L���O��ʂɈڍs
		pFade->SetFade(MODE_RANKING);
	}
#endif
}
//=============================================================================
// �`�揈��
//=============================================================================
void CResult::Draw(void)
{


}
