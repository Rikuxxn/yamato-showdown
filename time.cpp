//=============================================================================
//
// �^�C������ [time.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "time.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CTime::CTime(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	for (int nCnt = 0; nCnt < DIGITS; nCnt++)
	{
		m_apNumber[nCnt] = {};					// �e���̐����\���p
	}
	m_nMinutes = 0;								// ��
	m_nSeconds = 0;								// �b
	m_nFrameCount = 0;							// �t���[���J�E���g
	m_digitWidth = 0.0f;						// ����1��������̕�
	m_digitHeight = 0.0f;						// ����1��������̍���
	m_basePos = INIT_VEC3;						// �\���̊J�n�ʒu
	m_pColon = NULL;							// �R����
	m_nIdxTexture = NULL;						// �e�N�X�`���C���f�b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CTime::~CTime()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CTime* CTime::Create(int minutes, int seconds,float baseX,float baseY,float digitWidth,float digitHeight)
{
	CTime* pTime;

	pTime = new CTime;

	pTime->m_nMinutes = minutes;
	pTime->m_nSeconds = seconds;
	pTime->m_nFrameCount = 0;
	pTime->m_basePos = D3DXVECTOR3(baseX, baseY, 0.0f);
	pTime->m_digitWidth = digitWidth;
	pTime->m_digitHeight = digitHeight;

	// ����������
	pTime->Init();

	return pTime;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CTime::Init(void)
{
	// �R�����̕�
	float colonWidth = m_digitWidth / 2;

	// ���̐����i0,1���j�͂��̂܂�
	for (int nCnt = 0; nCnt < 2; nCnt++)
	{
		float posX = m_basePos.x + nCnt * m_digitWidth;
		float posY = m_basePos.y;

		// �i���o�[�̐���
		m_apNumber[nCnt] = CNumber::Create(posX, posY, m_digitWidth, m_digitHeight);

		if (!m_apNumber[nCnt])
		{
			return E_FAIL;
		}
	}

	// �b�̐����̓R�����̕������E�ɂ��炷
	for (int nCnt = 2; nCnt < DIGITS; nCnt++)
	{
		float posX = m_basePos.x + colonWidth + nCnt * m_digitWidth;
		float posY = m_basePos.y;

		m_apNumber[nCnt] = CNumber::Create(posX, posY, m_digitWidth, m_digitHeight);

		if (!m_apNumber[nCnt])
		{
			return E_FAIL;
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
void CTime::Uninit(void)
{
	for (int nCnt = 0; nCnt < DIGITS; nCnt++)
	{
		if (m_apNumber[nCnt] != NULL)
		{
			// �i���o�[�̏I������
			m_apNumber[nCnt]->Uninit();

			delete m_apNumber[nCnt];
			m_apNumber[nCnt] = NULL;
		}
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CTime::Update(void)
{
	// �^�C�}�[�J�E���g����
	Count();

	// �e���̕\���l���v�Z
	int min10 = m_nMinutes / 10;
	int min1 = m_nMinutes % 10;
	int sec10 = m_nSeconds / 10;
	int sec1 = m_nSeconds % 10;

	// �i���o�[�I�u�W�F�N�g�ɔ��f
	if (m_apNumber[0]) m_apNumber[0]->SetDigit(min10);
	if (m_apNumber[1]) m_apNumber[1]->SetDigit(min1);
	if (m_apNumber[2]) m_apNumber[2]->SetDigit(sec10);
	if (m_apNumber[3]) m_apNumber[3]->SetDigit(sec1);

	for (int nCnt = 0; nCnt < 4; nCnt++)
	{
		m_apNumber[nCnt]->Update();  // UV�X�V
	}
}
//=============================================================================
// �^�C�}�[�J�E���g����
//=============================================================================
void CTime::Count(void)
{
	if (CManager::GetMode() == MODE_RESULT)
	{
		return;
	}

	// �t���[���J�E���g
	m_nFrameCount++;

	// 60�t���[���o�߂�����1�b���Z
	if (m_nFrameCount >= 60)
	{
		m_nFrameCount = 0;

		m_nSeconds++;

		// 60�b��1���J��グ
		if (m_nSeconds >= 60)
		{
			m_nSeconds = 0;
			m_nMinutes++;
		}
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CTime::Draw(void)
{
	if (CManager::GetMode() == CScene::MODE_RESULT)
	{
		// �e�N�X�`���̎擾
		CTexture* pTexture = CManager::GetTexture();

		// �f�o�C�X�̎擾
		LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

		for (int nCnt = 0; nCnt < DIGITS; nCnt++)
		{
			if (m_apNumber[nCnt])
			{
				// �e�N�X�`���̐ݒ�
				pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

				m_apNumber[nCnt]->Draw();
			}
		}
	}
}
//=============================================================================
// �ʒu�̎擾
//=============================================================================
D3DXVECTOR3 CTime::GetPos(void)
{
	return D3DXVECTOR3();
}
//=============================================================================
// �R�����̃R���X�g���N�^
//=============================================================================
CColon::CColon(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff = NULL;						// ���_�o�b�t�@
	m_pos = D3DXVECTOR3(0.0f,0.0f,0.0f);	// �ʒu
	m_fWidth = 0.0f;						// ��
	m_fHeight = 0.0f;						// ����
	m_nIdxTexture = 0;
}
//=============================================================================
// �R�����̃f�X�g���N�^
//=============================================================================
CColon::~CColon()
{
	// �Ȃ�
}
//=============================================================================
// �R�����̐�������
//=============================================================================
CColon* CColon::Create(D3DXVECTOR3 pos, float fWidth, float fHeight)
{
	CColon* pColon;

	pColon = new CColon;

	pColon->m_pos = pos;
	pColon->m_fWidth = fWidth;
	pColon->m_fHeight = fHeight;

	// ����������
	pColon->Init();

	return pColon;
}
//=============================================================================
// �R�����̏���������
//=============================================================================
HRESULT CColon::Init(void)
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
void CColon::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != NULL)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = NULL;
	}

	CObject::Release();
}
//=============================================================================
// �R�����̍X�V����
//=============================================================================
void CColon::Update(void)
{




}
//=============================================================================
// �R�����̕`�揈��
//=============================================================================
void CColon::Draw(void)
{
	if (CManager::GetMode() == CScene::MODE_RESULT)
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
D3DXVECTOR3 CColon::GetPos(void)
{
	// �g��Ȃ�
	return D3DXVECTOR3();
}