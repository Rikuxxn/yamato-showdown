//=============================================================================
//
// �G�t�F�N�g���� [effect.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "effect.h"
#include "renderer.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CEffect::CEffect(int nPriority) : CObjectBillboard(nPriority)
{
	// �l�̃N���A
	m_move			= INIT_VEC3;// �ʒu
	m_fRadius		= 0.0f;		// ���a
	m_nLife			= 0;		// ����
	m_nIdxTexture	= 0;		// �e�N�X�`���C���f�b�N�X
	m_fGravity		= 0.0f;		// �d��
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CEffect::~CEffect()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CEffect* CEffect::Create(const EffectDesc& desc)
{
	// �G�t�F�N�g�I�u�W�F�N�g�̐���
	CEffect* pEffect = new CEffect;

	pEffect->SetPath(desc.path);

	// ����������
	pEffect->Init();

	pEffect->SetPos(desc.pos);
	pEffect->SetMove(desc.move);
	pEffect->SetCol(desc.col);
	pEffect->SetRadius(desc.fRadius);
	pEffect->SetLife(desc.nLife);
	pEffect->SetGravity(desc.fGravity);
	pEffect->SetDecRadius(desc.fDecRadius);

	return pEffect;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CEffect::Init(void)
{
	// �r���{�[�h�I�u�W�F�N�g�̏���������
	CObjectBillboard::Init();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CEffect::Uninit(void)
{
	// �r���{�[�h�I�u�W�F�N�g�̏I������
	CObjectBillboard::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CEffect::Update(void)
{
	// �r���{�[�h�I�u�W�F�N�g�̍X�V����
	CObjectBillboard::Update();

	m_move.y -= m_fGravity; // �d�͉����x

	// �ʒu�̎擾
	D3DXVECTOR3 Pos = GetPos();

	// �ʒu���X�V
	Pos += m_move;

	// �ʒu�̐ݒ�
	SetPos(Pos);
	SetSize(m_fRadius);

	m_fRadius -= m_fDecRadius;

	if (m_fRadius <= 0.0f)
	{
		m_fRadius = 0.0f;
	}

	m_nLife--;

	if (m_nLife <= 0)
	{
		// �G�t�F�N�g�̏I��
		Uninit();
		return;
	}
}
//=============================================================================
// �`�揈��
//=============================================================================
void CEffect::Draw(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ���e�X�g��L��
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//�f�t�H���g��false
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0���傫��������`��

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture));

	// �r���{�[�h�I�u�W�F�N�g�̕`�揈��
	CObjectBillboard::Draw();

	// ���e�X�g�𖳌��ɖ߂�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//�f�t�H���g��false

	// ���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}


//=============================================================================
// ���f���G�t�F�N�g�̃R���X�g���N�^
//=============================================================================
CModelEffect::CModelEffect()
{
	// �l�̃N���A
	m_nLife		= 0;			// �����i�b�j
	m_fGravity	= 0.0f;			// �d�͉����x
	m_scaleDec	= 0.0f;			// �T�C�Y������
	m_velocity	= INIT_VEC3;	// ���x
}
//=============================================================================
// ���f���G�t�F�N�g�̃f�X�g���N�^
//=============================================================================
CModelEffect::~CModelEffect()
{
	// �Ȃ�
}
//=============================================================================
// ���f���G�t�F�N�g�̐�������
//=============================================================================
CModelEffect* CModelEffect::Create(const char* path, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 velocity, D3DXVECTOR3 size, 
	int nLife, float fGravity, float scaleDec)
{
	CModelEffect* pEffect = new CModelEffect;

	pEffect->SetPath(path);

	// ����������
	pEffect->Init();

	pEffect->SetPos(pos);
	pEffect->SetRot(rot);
	pEffect->SetVelocity(velocity);
	pEffect->SetSize(size);
	pEffect->SetLife(nLife);
	pEffect->SetGravity(fGravity);
	pEffect->SetScaleDec(scaleDec);

	return pEffect;
}
//=============================================================================
// ���f���G�t�F�N�g�̏���������
//=============================================================================
HRESULT CModelEffect::Init(void)
{
	// X�t�@�C���I�u�W�F�N�g�̏���������
	CObjectX::Init();

	return S_OK;
}
//=============================================================================
// ���f���G�t�F�N�g�̏I������
//=============================================================================
void CModelEffect::Uninit(void)
{
	// X�t�@�C���I�u�W�F�N�g�̏I������
	CObjectX::Uninit();
}
//=============================================================================
// ���f���G�t�F�N�g�̍X�V����
//=============================================================================
void CModelEffect::Update(void)
{
	D3DXVECTOR3 Pos = GetPos();		// �ʒu�̎擾
	D3DXVECTOR3 Rot = GetRot();		// �����̎擾
	D3DXVECTOR3 Size = GetSize();	// �T�C�Y�̎擾

	if (m_nLife <= 0 || Size.x <= 0 || Size.y <= 0 || Size.z <= 0)
	{
		// �I������
		Uninit();
		return;
	}

	// ���������炷
	m_nLife--;

	// �T�C�Y�̌���
	Size.x -= m_scaleDec;
	Size.y -= m_scaleDec;
	Size.z -= m_scaleDec;

	// �d�͂����Z
	m_velocity.y += m_fGravity;

	// �ʒu���X�V
	Pos += m_velocity;

	SetPos(Pos);
	SetRot(Rot);
	SetSize(Size);

	// X�t�@�C���I�u�W�F�N�g�̍X�V����
	CObjectX::Update();
}
