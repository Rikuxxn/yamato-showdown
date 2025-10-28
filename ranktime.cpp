//=============================================================================
//
// �^�C������ [ranktime.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "ranktime.h"
#include "renderer.h"
#include "manager.h"
#include "time.h"
#include "rank.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CRankTime::CRankTime(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	memset(m_apNumber, 0, sizeof(m_apNumber));	// �e���̐����\���p
	memset(m_apRankClon, 0, sizeof(m_apRankClon));	// �R�����ւ̃|�C���^
	m_nMinutes = 0;								// ��
	m_nSeconds = 0;								// �b
	m_nFrameCount = 0;							// �t���[���J�E���g
	m_digitWidth = 0.0f;						// ����1��������̕�
	m_digitHeight = 0.0f;						// ����1��������̍���
	m_basePos = INIT_VEC3;						// �\���̊J�n�ʒu
	m_nIdxTexture = 0;							// �e�N�X�`���C���f�b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CRankTime::~CRankTime()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CRankTime* CRankTime::Create(float baseX,float baseY,float digitWidth,float digitHeight)
{
	CRankTime* pRankTime;

	pRankTime = new CRankTime;

	pRankTime->m_nFrameCount = 0;
	pRankTime->m_basePos = D3DXVECTOR3(baseX, baseY, 0.0f);
	pRankTime->m_digitWidth = digitWidth;
	pRankTime->m_digitHeight = digitHeight;

	// ����������
	pRankTime->Init();

	return pRankTime;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CRankTime::Init(void)
{
	// 5�ʂ܂Ő�������
	for (int i = 0; i < MaxRanking; i++)
	{
		// ����UI�̐���
		float UIbaseX = m_basePos.x + m_digitWidth;
		float UIbaseY = m_basePos.y + i * (m_digitHeight + 50.0f);

		// �e�X�g
		CRank::Create(D3DXVECTOR3(UIbaseX, UIbaseY, 0.0f), m_digitWidth / 2, m_digitHeight, (float)i);

		// ����UI�̕�
		float rankWidth = (m_digitWidth / 2) + 50.0f;

		// ��
		for (int n = 0; n < 2; n++)
		{
			float x = UIbaseX + rankWidth + n * m_digitWidth;
			float y = UIbaseY;

			m_apNumber[i][n] = CNumber::Create(x, y, m_digitWidth, m_digitHeight);
		}
		
		// �R�����̐����i���ƕb�̊ԁj
		float colonX = UIbaseX + rankWidth + 2 * m_digitWidth;
		float colonY = UIbaseY;
		m_apRankClon[i] = CRankColon::Create(D3DXVECTOR3(colonX, colonY, 0.0f), m_digitWidth / 2, m_digitHeight);

		// �R�����̕�
		float colonWidth = rankWidth + m_digitWidth / 2;

		// �b
		for (int n = 2; n < DIGITS; n++)
		{
			float x = UIbaseX + colonWidth + n * m_digitWidth;
			float y = UIbaseY;

			m_apNumber[i][n] = CNumber::Create(x, y, m_digitWidth, m_digitHeight);
		}
	}

	// �e�N�X�`�����蓖��
	CTexture* pTexture = CManager::GetTexture();
	m_nIdxTexture = pTexture->RegisterDynamic("data/TEXTURE/time.png");

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CRankTime::Uninit(void)
{
	for (int i = 0; i < MaxRanking; i++)
	{
		for (int n = 0; n < DIGITS; n++)
		{
			if (m_apNumber[i][n])
			{
				// �i���o�[�̏I������
				m_apNumber[i][n]->Uninit();

				delete m_apNumber[i][n];
				m_apNumber[i][n] = nullptr;
			}
		}
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CRankTime::Update(void)
{
	for (int i = 0; i < MaxRanking; i++)
	{
		for (int n = 0; n < DIGITS; n++)
		{
			if (m_apNumber[i][n])
			{
				// �i���o�[�̍X�V����(UV)
				m_apNumber[i][n]->Update();
			}
		}
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CRankTime::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int i = 0; i < MaxRanking; i++)
	{
		for (int n = 0; n < DIGITS; n++)
		{
			if (m_apNumber[i][n])
			{
				// �e�N�X�`���̐ݒ�
				pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

				// �i���o�[�̕`�揈��
				m_apNumber[i][n]->Draw();
			}
		}
	}
}
//=============================================================================
// �����L���O���X�g�̐ݒ菈��
//=============================================================================
void CRankTime::SetRankList(const std::vector<std::pair<int, int>>& rankList)
{
	for (int i = 0; i < rankList.size() && i < MaxRanking; i++)
	{
		int min10 = rankList[i].first / 10;
		int min1 = rankList[i].first % 10;
		int sec10 = rankList[i].second / 10;
		int sec1 = rankList[i].second % 10;

		if (m_apNumber[i][0])m_apNumber[i][0]->SetDigit(min10);
		if (m_apNumber[i][1])m_apNumber[i][1]->SetDigit(min1);
		if (m_apNumber[i][2])m_apNumber[i][2]->SetDigit(sec10);
		if (m_apNumber[i][3])m_apNumber[i][3]->SetDigit(sec1);
	}
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CRankTime::GetPos(void)
{
	return D3DXVECTOR3();
}


//=============================================================================
// �R�����̃R���X�g���N�^
//=============================================================================
CRankColon::CRankColon(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff = nullptr;					// ���_�o�b�t�@
	m_pos = D3DXVECTOR3(0.0f,0.0f,0.0f);	// �ʒu
	m_fWidth = 0.0f;						// ��
	m_fHeight = 0.0f;						// ����
	m_nIdxTexture = 0;
}
//=============================================================================
// �R�����̃f�X�g���N�^
//=============================================================================
CRankColon::~CRankColon()
{
	// �Ȃ�
}
//=============================================================================
// �R�����̐�������
//=============================================================================
CRankColon* CRankColon::Create(D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CRankColon* pRankColon;

	pRankColon = new CRankColon;

	pRankColon->m_pos = pos;
	pRankColon->m_fWidth = fWidth;
	pRankColon->m_fHeight = fHeight;

	// ����������
	pRankColon->Init();

	return pRankColon;
}
//=============================================================================
// �R�����̏���������
//=============================================================================
HRESULT CRankColon::Init(void)
{
	// �f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	m_nIdxTexture = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/colon.png");

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_2D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(m_pos.x, m_pos.y, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(m_pos.x, m_pos.y + m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(m_pos.x + m_fWidth, m_pos.y + m_fHeight, 0.0f);

	// rhw�̐ݒ�
	pVtx[0].rhw = 1.0f;
	pVtx[1].rhw = 1.0f;
	pVtx[2].rhw = 1.0f;
	pVtx[3].rhw = 1.0f;

	// ���_�J���[�̐ݒ�
	pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	// �e�N�X�`�����W�̐ݒ�
	pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// �R�����̏I������
//=============================================================================
void CRankColon::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	this->Release();
}
//=============================================================================
// �R�����̍X�V����
//=============================================================================
void CRankColon::Update(void)
{




}
//=============================================================================
// �R�����̕`�揈��
//=============================================================================
void CRankColon::Draw(void)
{
	if (CManager::GetMode() == CScene::MODE_RANKING)
	{
		// �e�N�X�`���̎擾
		CTexture* pTexture = CManager::GetTexture();

		// �f�o�C�X�̎擾
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
		pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

		// ���_�t�H�[�}�b�g�̐ݒ�
		pDevice->SetFVF(FVF_VERTEX_2D);

		// �e�N�X�`���̐ݒ�
		pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

		// �|���S���̕`��
		pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
	}
}
//=============================================================================
// �R�����̈ʒu�擾����
//=============================================================================
D3DXVECTOR3 CRankColon::GetPos(void)
{
	// �g��Ȃ�
	return D3DXVECTOR3();
}