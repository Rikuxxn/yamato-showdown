//=============================================================================
//
// �r���{�[�h���� [objectBillboard.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectBillboard.h"
#include "renderer.h"
#include "manager.h"
#include "game.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CObjectBillboard::CObjectBillboard(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	memset(m_szPath, 0, sizeof(m_szPath));	// �t�@�C���p�X
	m_pVtxBuff		= nullptr;				// ���_�o�b�t�@�ւ̃|�C���^
	m_pos			= INIT_VEC3;			// �ʒu
	m_col			= INIT_XCOL;			// �F
	m_mtxWorld		= {};					// ���[���h�}�g���b�N�X
	m_fSize			= 0.0f;					// �T�C�Y(�G�t�F�N�g���a)
	m_fHeight		= 0.0f;					// �T�C�Y(�r���{�[�h)
	m_nIdxTexture	= 0;					// �e�N�X�`���C���f�b�N�X
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CObjectBillboard::~CObjectBillboard()
{
	// �Ȃ�
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CObjectBillboard::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �e�N�X�`���̎擾
	m_nIdxTexture = CManager::GetTexture()->RegisterDynamic(m_szPath);

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	VERTEX_3D* pVtx = nullptr;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(+m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fSize, -m_fSize - m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(+m_fSize, -m_fSize - m_fHeight, 0.0f);

	// �e���_�̖@���̐ݒ�
	pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
	pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

	// ���_�J���[�̐ݒ�
	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

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
void CObjectBillboard::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CObjectBillboard::Update(void)
{
	VERTEX_3D* pVtx;// ���_���ւ̃|�C���^

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	// ���_���W�̐ݒ�
	pVtx[0].pos = D3DXVECTOR3(-m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[1].pos = D3DXVECTOR3(+m_fSize, +m_fSize + m_fHeight, 0.0f);
	pVtx[2].pos = D3DXVECTOR3(-m_fSize, -m_fSize - m_fHeight, 0.0f);
	pVtx[3].pos = D3DXVECTOR3(+m_fSize, -m_fSize - m_fHeight, 0.0f);

	pVtx[0].col = m_col;
	pVtx[1].col = m_col;
	pVtx[2].col = m_col;
	pVtx[3].col = m_col;

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CObjectBillboard::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���C�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// Z�e�X�g
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);	// Z�̔�r���@
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);		// Z�o�b�t�@�ɏ������܂Ȃ�

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �r���[�}�g���b�N�X�̎擾
	D3DXMATRIX mtxView;
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	// �J�����̋t�s���ݒ�
	m_mtxWorld._11 = mtxView._11;
	m_mtxWorld._12 = mtxView._21;
	m_mtxWorld._13 = mtxView._31;
	m_mtxWorld._21 = mtxView._12;
	m_mtxWorld._22 = mtxView._22;
	m_mtxWorld._23 = mtxView._32;
	m_mtxWorld._31 = mtxView._13;
	m_mtxWorld._32 = mtxView._23;
	m_mtxWorld._33 = mtxView._33;

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	// ���ɖ߂�
	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);//Z�̔�r���@
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);//Z�o�b�t�@�ɏ�������

	// ���C�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}


//=============================================================================
// �q���g�r���{�[�h�̃R���X�g���N�^
//=============================================================================
CHintBillboard::CHintBillboard()
{
	// �l�̃N���A
	m_fAlpha	= 0.0f;			// �A���t�@�l
	m_state		= STATE_NORMAL;	// ���
}
//=============================================================================
// �q���g�r���{�[�h�̃f�X�g���N�^
//=============================================================================
CHintBillboard::~CHintBillboard()
{
	// �Ȃ�
}
//=============================================================================
// �q���g�r���{�[�h�̏���������
//=============================================================================
HRESULT CHintBillboard::Init(void)
{
	// �r���{�[�h�̏���������
	CObjectBillboard::Init();

	return S_OK;
}
//=============================================================================
// �q���g�r���{�[�h�̏I������
//=============================================================================
void CHintBillboard::Uninit(void)
{
	// �r���{�[�h�̏I������
	CObjectBillboard::Uninit();
}
//=============================================================================
// �q���g�r���{�[�h�̍X�V����
//=============================================================================
void CHintBillboard::Update(void)
{
	// �r���{�[�h�̍X�V����
	CObjectBillboard::Update();

	if (!CGame::GetPlayer())
	{
		return;
	}

	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();
	D3DXVECTOR3 disPos = playerPos - GetPos();
	float distance = D3DXVec3Length(&disPos);

	const float kTriggerDistance = 200.0f; // �\������

	switch (m_state)
	{
	case STATE_FADEIN:
		m_fAlpha += 0.02f;

		if (m_fAlpha > 1.0f)
		{
			m_fAlpha = 1.0f;
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));

		break;
	case STATE_NORMAL:
		if (distance < kTriggerDistance)
		{// ���X�ɒʏ�ɖ߂����
			m_state = STATE_FADEIN;
		}
		else
		{// ���X�ɓ����ɂ�����
			m_state = STATE_FADEOUT;
		}
		break;
	case STATE_FADEOUT:
		m_fAlpha -= 0.02f;

		if (m_fAlpha < 0.0f)
		{
			m_fAlpha = 0.0f;
			m_state = STATE_NORMAL;
		}

		SetCol(D3DXCOLOR(1.0f, 1.0f, 1.0f, m_fAlpha));
		break;
	}
}
//=============================================================================
// �q���g�r���{�[�h�̕`�揈��
//=============================================================================
void CHintBillboard::Draw(void)
{
	// �r���{�[�h�̕`�揈��
	CObjectBillboard::Draw();
}
