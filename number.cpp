//=============================================================================
//
// �i���o�[���� [number.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "number.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CNumber::CNumber()
{
	// �l�̃N���A
	m_pVtxBuff	= nullptr;	// ���_�o�b�t�@�ւ̃|�C���^
	m_pos		= INIT_VEC3;// �ʒu
	m_col		= INIT_COL;	// �F
	m_digit		= 0;		// ��
	m_fWidth	= 0.0f;		// ��
	m_fHeight	= 0.0f;		// ����
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CNumber::~CNumber()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CNumber* CNumber::Create(float fposX, float fposY, float fWidth, float fHeight)
{
	// �I�u�W�F�N�g�̐���
	CNumber* pNumber = new CNumber;

	// ����������
	pNumber->Init(fposX, fposY, fWidth, fHeight);

	return pNumber;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CNumber::Init(float fposX, float fposY, float fWidth, float fHeight)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �l�̕ۑ�
	m_pos = D3DXVECTOR3(fposX, fposY, 0.0f);
	m_fWidth = fWidth;
	m_fHeight = fHeight;
	m_col = D3DCOLOR_ARGB(255, 255, 255, 255);
	m_digit = 0;

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
	pVtx[0].pos = D3DXVECTOR3(fposX, fposY, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(fposX + fWidth, fposY, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(fposX, fposY + fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(fposX + fWidth, fposY + fHeight, 0.0f);

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
// �I������
//=============================================================================
void CNumber::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CNumber::Update(void)
{
	// UV�v�Z�i1��10�����̏ꍇ�j
	const float digitWidthUV = 1.0f / 10.0f;
	float tu = m_digit * digitWidthUV;
	float tu2 = tu + digitWidthUV;

	VERTEX_2D* pVtx;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// UV���W�̍X�V�i�e�N�X�`���̈ꕔ�𐔎����Ƃɐ؂�ւ���j
	pVtx[0].tex = D3DXVECTOR2(tu, 0.0f);   // ����
	pVtx[1].tex = D3DXVECTOR2(tu2, 0.0f);  // �E��
	pVtx[2].tex = D3DXVECTOR2(tu, 1.0f);   // ����
	pVtx[3].tex = D3DXVECTOR2(tu2, 1.0f);  // �E��

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CNumber::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, nullptr);
}
//=============================================================================
// �����ݒ菈��
//=============================================================================
void CNumber::SetDigit(int digit)
{
	// �͈̓`�F�b�N
	if (digit < 0 || digit > 9)
	{
		digit = 0;
	}

	m_digit = digit;
}
//=============================================================================
// �ʒu�̐ݒ菈��
//=============================================================================
void CNumber::SetPos(D3DXVECTOR3 pos)
{
	m_pos = pos;
}
