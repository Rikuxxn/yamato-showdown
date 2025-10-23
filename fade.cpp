//=============================================================================
//
// �t�F�[�h���� [fade.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "fade.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CFade::CFade()
{
	// �l�̃N���A
	m_pVtxBuff	= nullptr;			// ���_�o�b�t�@
	m_pos		= INIT_VEC3;		// �ʒu
	m_fade		= FADE_NONE;		// �t�F�[�h�̏��
	m_fadeCol	= INIT_XCOL;		// �t�F�[�h�J���[
	m_SceneNext = CScene::MODE_MAX;	// ���̉��
	m_fWidth	= 0.0f;				// ��
	m_fHeight	= 0.0f;				// ����
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CFade::~CFade()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CFade* CFade::Create(CScene::MODE mode)
{
	CFade* pFade = nullptr;

	pFade = new CFade;

	// ����������
	pFade->Init(mode);

	return pFade;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CFade::Init(CScene::MODE mode)
{
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

	m_fade = FADE_IN;
	m_SceneNext = mode;	//���̉��
	m_fadeCol = D3DXCOLOR(0.0f, 0.0f, 0.0f, 1.0f);

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
	pVtx[0].col = D3DXCOLOR(m_fadeCol);
	pVtx[1].col = D3DXCOLOR(m_fadeCol);
	pVtx[2].col = D3DXCOLOR(m_fadeCol);
	pVtx[3].col = D3DXCOLOR(m_fadeCol);

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
void CFade::Uninit(void)
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
void CFade::Update(void)
{
	VERTEX_2D* pVtx;

	if (m_fade != FADE_NONE)
	{
		if (m_fade == FADE_IN)
		{//�t�F�[�h�C�����
			m_fadeCol.a -= 0.03f;//�|���S���𓧖��ɂ��Ă���

			if (m_fadeCol.a <= 0.0f)
			{
				m_fadeCol.a = 0.0f;
				m_fade = FADE_NONE;
			}
		}
		else if (m_fade == FADE_OUT)
		{
			m_fadeCol.a += 0.03f;

			if (m_fadeCol.a >= 1.0f)
			{
				m_fadeCol.a = 1.0f;
				m_fade = FADE_IN;

				//���[�h�ݒ�
				CManager::SetMode(m_SceneNext);

				return;
			}
		}

		//���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
		m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

		//���_�J���[�̐ݒ�
		pVtx[0].col = D3DXCOLOR(m_fadeCol);
		pVtx[1].col = D3DXCOLOR(m_fadeCol);
		pVtx[2].col = D3DXCOLOR(m_fadeCol);
		pVtx[3].col = D3DXCOLOR(m_fadeCol);

		//���_�o�b�t�@���A�����b�N����
		m_pVtxBuff->Unlock();
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CFade::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_2D));

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_2D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, nullptr);

	// �|���S���̕`��
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}
//=============================================================================
// �t�F�[�h�̐ݒ菈��
//=============================================================================
void CFade::SetFade(CScene::MODE modeNext)
{
	m_fade = FADE_OUT;		//�t�F�[�h�A�E�g��Ԃ�
	m_SceneNext = modeNext;	//���̉�ʂ�ݒ�
}
