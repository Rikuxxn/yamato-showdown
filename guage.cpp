//=============================================================================
//
// �Q�[�W���� [guage.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "guage.h"
#include "manager.h"
#include "game.h"
#include "charactermanager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGuage::CGuage()
{
	// �l�̃N���A
	m_nIdxTexture = 0;			// �e�N�X�`���C���f�b�N�X
	m_type = TYPE_NONE;			// �Q�[�W�̎��
	m_fWidth = 0.0f;			// ����
	m_fHeight = 0.0f;			// �c��
	m_targetRate = 0.0f;		// ���ۂ�HP����
	m_currentRate = 0.0f;		// �\���p�Q�[�W�����i�Ǐ]�p�j
	m_speed = 0.0f;				// �Ǐ]���x
	m_delayTimer = 0;			// �x���^�C�}�[(�o�b�N�Q�[�W�p)
	m_pTargetChar = nullptr; // ���̃Q�[�W���Ǐ]����L�����N�^�[
}
//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGuage::~CGuage()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CGuage* CGuage::Create(GUAGETYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CGuage* pGuage = new CGuage;

	// ����������
	pGuage->SetPos(pos);
	pGuage->SetSize(fWidth, fHeight);
	pGuage->m_type = type;
	pGuage->m_fWidth = fWidth;
	pGuage->m_fHeight = fHeight;

	if (type == TYPE_GUAGE)
	{// �Q�[�W�͗�
		pGuage->SetCol(D3DXCOLOR(0.0f, 1.0f, 0.0f, 1.0f));
	}
	else if (type == TYPE_BACKGUAGE)
	{// �o�b�N�Q�[�W�͐�
		pGuage->SetCol(D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f));
	}
	else if (type == TYPE_FRAME)
	{// �t���[���̓e�N�X�`���̐F���g���̂Ŕ�
		pGuage->SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f));
	}

	pGuage->m_targetRate = 1.0f;
	pGuage->m_currentRate = 1.0f;
	pGuage->m_speed = 0.005f; // �Ǐ]���x

	// ����������
	pGuage->Init();

	return pGuage;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CGuage::Init(void)
{
	if (m_type == TYPE_FRAME)
	{
		// �e�N�X�`���ݒ�
		m_nIdxTexture = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/HpFrame.png");
	}

	// 2D�I�u�W�F�N�g�̏���������
	CObject2D::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CGuage::Uninit(void)
{
	// 2D�I�u�W�F�N�g�̏I������
	CObject2D::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CGuage::Update(void)
{
	// ���O���std�̎g�p
	using namespace std;

	if (m_type == TYPE_FRAME) 
	{
		UpdateFrame();
		return;
	}

	float fRate = m_pTargetChar->GetHp() / m_pTargetChar->GetMaxHp();
	fRate = max(0.0f, min(fRate, 1.0f));

	if (m_type == TYPE_GUAGE)
	{
		// ���_���W���X�V
		UpdateGuageVtx(fRate);
	}
	else if (m_type == TYPE_BACKGUAGE)
	{
		// �ԃo�b�N�Q�[�W�͏����x��ĒǏ]
		m_targetRate = fRate;

		if (m_currentRate > m_targetRate)
		{
			m_currentRate -= m_speed;

			if (m_currentRate < m_targetRate)
			{
				m_currentRate = m_targetRate;
			}
		}
		else if (m_currentRate < m_targetRate)
		{
			m_currentRate += m_speed;

			if (m_currentRate > m_targetRate)
			{
				m_currentRate = m_targetRate;
			}
		}

		// ���_���W���X�V
		UpdateGuageVtx(m_currentRate);
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGuage::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (m_type == TYPE_FRAME)
	{
		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));
	}
	else
	{// �Q�[�W���̂ɂ͓K�p���Ȃ�
		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, nullptr);
	}

	// 2D�I�u�W�F�N�g�̕`�揈��
	CObject2D::Draw();
}
