//=============================================================================
//
// �X�e���V���V���h�E���� [shadowS.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "shadowS.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CShadowS::CShadowS(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff	= nullptr;		// ���_�o�b�t�@�ւ̃|�C���^
	m_pos		= INIT_VEC3;	// �ʒu
	m_col		= INIT_XCOL;	// �F
	m_fWidth	= 0.0f;			// ��
	m_fHeight	= 0.0f;			// ����
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CShadowS::~CShadowS()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CShadowS* CShadowS::Create(const char* pFilepath,D3DXVECTOR3 /*pos*/)
{
	CShadowS* pShadowS = new CShadowS;

	pShadowS->SetPath(pFilepath);

	// ����������
	pShadowS->Init();

	return pShadowS;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CShadowS::Init(void)
{
	// X�t�@�C���I�u�W�F�N�g�̏���������
	CObjectX::Init();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_2D) * 4,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_2D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// ������
	m_pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	m_fWidth = 1920.0f;
	m_fHeight = 1080.0f;

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
	pVtx[0].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pVtx[1].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pVtx[2].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);
	pVtx[3].col = D3DXCOLOR(0.0f, 0.0f, 0.0f, 0.5f);

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
void CShadowS::Uninit(void)
{
	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// X�t�@�C���I�u�W�F�N�g�̔j��
	CObjectX::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CShadowS::Update(void)
{
	// X�t�@�C���I�u�W�F�N�g�̍X�V����
	CObjectX::Update();

	// �ʒu�̐ݒ�
	SetPos(m_pos);
}
//=============================================================================
// �`�揈��
//=============================================================================
void CShadowS::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �X�e���V���e�X�g��L���ɂ���
	pDevice->SetRenderState(D3DRS_STENCILENABLE, TRUE);

	// Z�o�b�t�@�ւ̏������݂𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);

	// �J���[�o�b�t�@�ւ̏������݂𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x00000000);

	// �X�e���V���o�b�t�@�̔�r�p�����[�^��ݒ�
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_ALWAYS);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_KEEP);	// �X�e���V���e�X�g : ���i / Z�e�X�g : ���i
	//pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT);// �X�e���V���e�X�g : ���i / Z�e�X�g : �s���i
	//pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);	// �X�e���V���e�X�g : �s���i

	// �\�ʂ��J�����O
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

	// X�I�u�W�F�N�g�̕`�揈��
	CObjectX::Draw();

	// �X�e���V���o�b�t�@�̎Q�ƒl��ݒ�(1�ɂ���)
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x01);

	// �X�e���V���o�b�t�@�̔�r�p�����[�^��ݒ�
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);	// �X�e���V���e�X�g : ���i / Z�e�X�g : ���i
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_KEEP);		// �X�e���V���e�X�g : ���i / Z�e�X�g : �s���i
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);	// �X�e���V���e�X�g : �s���i

	// ���ʂ��J�����O
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// X�I�u�W�F�N�g�̕`�揈��
	CObjectX::Draw();

	// �J���[�o�b�t�@�ւ̏������݂�L���ɂ���
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// �X�e���V���o�b�t�@�̎Q�ƒl��ݒ�(2�ɂ���)
	pDevice->SetRenderState(D3DRS_STENCILREF, 0x02);

	// �X�e���V���o�b�t�@�̔�r�p�����[�^��ݒ�
	pDevice->SetRenderState(D3DRS_STENCILFUNC, D3DCMP_EQUAL);
	pDevice->SetRenderState(D3DRS_STENCILPASS, D3DSTENCILOP_INCRSAT);	// �X�e���V���e�X�g : ���i / Z�e�X�g : ���i
	pDevice->SetRenderState(D3DRS_STENCILZFAIL, D3DSTENCILOP_INCRSAT);	// �X�e���V���e�X�g : ���i / Z�e�X�g : �s���i
	pDevice->SetRenderState(D3DRS_STENCILFAIL, D3DSTENCILOP_INCRSAT);	// �X�e���V���e�X�g : �s���i

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �����|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

	//*****************************************************************************
	// ��Ԃ����ɖ߂�
	//*****************************************************************************

	// �X�e���V���e�X�g�𖳌��ɂ���
	pDevice->SetRenderState(D3DRS_STENCILENABLE, FALSE);

	// Z�o�b�t�@�ւ̏������݂�L���ɂ���
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);

	// �J���[�o�b�t�@�ւ̏������݂�L���ɂ���
	pDevice->SetRenderState(D3DRS_COLORWRITEENABLE, 0x0000000F);

	// �J�����O�ݒ��L����
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
}
