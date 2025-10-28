//=============================================================================
//
// �^�C������ [ranktime.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "rank.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R�����̃R���X�g���N�^
//=============================================================================
CRank::CRank(int nPriority) : CObject(nPriority)
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
CRank::~CRank()
{
	// �Ȃ�
}
//=============================================================================
// �R�����̐�������
//=============================================================================
CRank* CRank::Create(D3DXVECTOR3 pos, float fWidth, float fHeight, float fRank)
{
	CRank* pRank;

	pRank = new CRank;

	pRank->m_pos = pos;
	pRank->m_fWidth = fWidth;
	pRank->m_fHeight = fHeight;
	pRank->m_fRank = fRank;

	// ����������
	pRank->Init();

	return pRank;
}
//=============================================================================
// �R�����̏���������
//=============================================================================
HRESULT CRank::Init(void)
{
	// �f�o�C�X�擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	m_nIdxTexture = CManager::GetTexture()->RegisterDynamic("data/TEXTURE/rank.png");

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
	pVtx[0].tex = D3DXVECTOR2(m_fRank * 0.2f, 0.0f);
	pVtx[1].tex = D3DXVECTOR2(m_fRank * 0.2f + 0.2f, 0.0f);
	pVtx[2].tex = D3DXVECTOR2(m_fRank * 0.2f, 1.0f);
	pVtx[3].tex = D3DXVECTOR2(m_fRank * 0.2f + 0.2f, 1.0f);

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// �R�����̏I������
//=============================================================================
void CRank::Uninit(void)
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
void CRank::Update(void)
{




}
//=============================================================================
// �R�����̕`�揈��
//=============================================================================
void CRank::Draw(void)
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