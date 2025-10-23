//=============================================================================
//
// �O���b�h���� [grid.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "grid.h"
#include "manager.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGrid::CGrid()
{
	// �l�̃N���A
    m_mtxWorld  = {};   // ���[���h�}�g���b�N�X
    m_vertices  = {};   // ���_
    m_size      = 0;    // ���a
    m_spacing   = 0.0f; // �}�X�Ԋu
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CGrid::~CGrid()
{
	// ���X�g�̃N���A
	m_vertices.clear();
}
//=============================================================================
// ���j�[�N�|�C���^�̐���
//=============================================================================
std::unique_ptr<CGrid> CGrid::Create(void)
{
    return make_unique<CGrid>();
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CGrid::Init(void)
{
	m_size      = SIZE;
	m_spacing   = SPACING;

	// ���X�g�̃N���A
	m_vertices.clear();

    // X�������̐�
    for (int nCnt = -m_size; nCnt <= m_size; nCnt++)
    {
        AddLine(D3DXVECTOR3(-m_size * m_spacing, 0, nCnt * m_spacing), 
            D3DXVECTOR3(m_size * m_spacing, 0, nCnt * m_spacing),
            D3DCOLOR_XRGB(128,128,128));
    }

    // Z�������̐�
    for (int nCnt = -m_size; nCnt <= m_size; nCnt++)
    {
        AddLine(D3DXVECTOR3(nCnt * m_spacing, 0, -m_size * m_spacing), 
            D3DXVECTOR3(nCnt * m_spacing, 0, m_size * m_spacing),
            D3DCOLOR_XRGB(128,128,128));
    }

    // �}�j�s�����[�^�̕`��
    AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(150, 0, 0), D3DCOLOR_XRGB(255, 0, 0));// X��
    AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 150, 0), D3DCOLOR_XRGB(0, 255, 0));// Y��
    AddLine(D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 150), D3DCOLOR_XRGB(0, 0, 255));// Z��

	return S_OK;
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGrid::Draw(void)
{
    // �f�o�C�X�̎擾
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

    pDevice->SetRenderState(D3DRS_LIGHTING, FALSE); // ���C�e�B���O����

    // ���_�t�H�[�}�b�g�̐ݒ�
    pDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);

    // �e�N�X�`���̐ݒ�
    pDevice->SetTexture(0, nullptr);

    // �`��
    pDevice->DrawPrimitiveUP(D3DPT_LINELIST, m_vertices.size() / 2, m_vertices.data(), sizeof(CUSTOMVERTEX));

    pDevice->SetRenderState(D3DRS_LIGHTING, TRUE); // ���C�e�B���O��߂�
}
//=============================================================================
// ���C���ǉ�����
//=============================================================================
void CGrid::AddLine(const D3DXVECTOR3& p1, const D3DXVECTOR3& p2, D3DCOLOR col)
{
    m_vertices.push_back({ p1,col });
    m_vertices.push_back({ p2,col });
}