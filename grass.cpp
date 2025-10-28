//=============================================================================
//
// ������ [grass.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "grass.h"
#include "manager.h"
#include "game.h"
#include "easing.h"
#include "player.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CGrassBlock::CGrassBlock(int nPriority) : CBlock(nPriority)
{
	// �l�̃N���A
	m_rotVel = INIT_VEC3;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CGrassBlock::~CGrassBlock()
{
	// �Ȃ�
}
//=============================================================================
// �X�V����
//=============================================================================
void CGrassBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	if (!CGame::GetPlayer())
	{
		return;
	}

	// �v���C���[�̈ʒu���擾
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();

	// ���ƃv���C���[�̋���
	D3DXVECTOR3 diff = playerPos - GetPos();
	float dist = D3DXVec3Length(&diff);

	D3DXVECTOR3 rot = GetRot();
	float distMax = 60.0f;				// ���̋����ȓ��Ȃ�X��
	float fMaxTilt = D3DXToRadian(60);  // �ő�̌X���p�x

	// �ڕW�̉�]�p�x
	D3DXVECTOR3 targetRot = D3DXVECTOR3(0, 0, 0);

	if (dist < distMax) // �͈͓�
	{
		// ���K��
		D3DXVec3Normalize(&diff, &diff);
		
		// �����ɉ���������
		float t = 1.0f - (dist / distMax);

		// �X���p�x = �ő�p�x �~ ����
		float tilt = fMaxTilt * t;

		rot.x = -diff.z * tilt;
		rot.z = diff.x * tilt;
	}

	// *** �o�l �{ �_���s���O ***
	float stiffness = 0.12f;		// �o�l�̋����i0.1�`0.3���炢�j
	float damping = 0.75f;			// �������i0.8�`0.98���炢�j

	// �o�l�� = (�ڕW - ����) �~ �o�l�萔
	m_rotVel.x += (targetRot.x - rot.x) * stiffness;
	m_rotVel.z += (targetRot.z - rot.z) * stiffness;

	// �����i��R�j
	m_rotVel.x *= damping;
	m_rotVel.z *= damping;

	// ��]�ɔ��f
	rot.x += m_rotVel.x;
	rot.z += m_rotVel.z;

	// �����̐ݒ�
	SetRot(rot);
}
//=============================================================================
// �`�揈��
//=============================================================================
void CGrassBlock::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// ���e�X�g��L��
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//�f�t�H���g��false
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0���傫��������`��

	// �u���b�N�̕`��
	CBlock::Draw();

	// ���e�X�g�𖳌��ɖ߂�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//�f�t�H���g��false

	// ���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
