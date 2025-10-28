//=============================================================================
//
// �u���b�N���X�g���� [blocklist.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "blocklist.h"
#include "blockmanager.h"
#include "player.h"
#include "game.h"
#include "manager.h"
#include "particle.h"
#include <algorithm>
#include "meshcylinder.h"
#include "collisionUtils.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �ؔ��u���b�N�̃R���X�g���N�^
//=============================================================================
CWoodBoxBlock::CWoodBoxBlock()
{
	// �l�̃N���A
	m_ResPos = INIT_VEC3;// ���X�|�[���ʒu
}
//=============================================================================
// �ؔ��u���b�N�̃f�X�g���N�^
//=============================================================================
CWoodBoxBlock::~CWoodBoxBlock()
{
	// �Ȃ�
}
//=============================================================================
// �ؔ��u���b�N�̏���������
//=============================================================================
HRESULT CWoodBoxBlock::Init(void)
{
	// �u���b�N�̏���������
	CBlock::Init();

	// �ŏ��̈ʒu�����X�|�[���ʒu�ɐݒ�
	m_ResPos = GetPos();

	// ���I�ɖ߂�
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// �ؔ��u���b�N�̍X�V����
//=============================================================================
void CWoodBoxBlock::Update()
{
	CBlock::Update(); // ���ʏ���
}
//=============================================================================
// ���X�|�[������
//=============================================================================
void CWoodBoxBlock::Respawn(D3DXVECTOR3 resPos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// �����Ԃ��p�[�e�B�N������
	CParticle::Create<CWaterParticle>(INIT_VEC3, GetPos(), D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.8f), 50, 10);
	CParticle::Create<CWaterParticle>(INIT_VEC3, GetPos(), D3DXCOLOR(0.3f, 0.5f, 1.0f, 0.5f), 50, 10);

	// ���X�|�[���ʒu�ɐݒ�
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// �R���C�_�[�̍X�V
	UpdateCollider();

	// ���I�ɖ߂�
	SetEditMode(false);
}
//=============================================================================
// �Z�b�g����
//=============================================================================
void CWoodBoxBlock::Set(D3DXVECTOR3 pos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ���u���b�N�̈ʒu���擾
	D3DXVECTOR3 Pos = GetPos();
	D3DXVECTOR3 Rot = GetRot();

	D3DXVECTOR3 targetPos(pos);			// �ݒu����ڕW�ʒu
	D3DXVECTOR3 rot(0.0f, 0.0f, 0.0f);	// ���������Z�b�g

	Pos = targetPos;
	Rot = rot;

	SetPos(Pos);
	SetRot(Rot);

	// �R���C�_�[�̍X�V
	UpdateCollider();
}


//=============================================================================
// ���ău���b�N�̃R���X�g���N�^
//=============================================================================
CTorchBlock::CTorchBlock()
{
	// �l�̃N���A
}
//=============================================================================
// ���ău���b�N�̃f�X�g���N�^
//=============================================================================
CTorchBlock::~CTorchBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���ău���b�N�̍X�V����
//=============================================================================
void CTorchBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

}


//=============================================================================
// ���u���b�N�̃R���X�g���N�^
//=============================================================================
CWaterBlock::CWaterBlock(int nPriority) : CBlock(nPriority)
{
	// �l�̃N���A
	m_counter = 0;// �����J�E���^�[
}
//=============================================================================
// ���u���b�N�̃f�X�g���N�^
//=============================================================================
CWaterBlock::~CWaterBlock()
{
	// �Ȃ�
}
//=============================================================================
// ���u���b�N�̍X�V����
//=============================================================================
void CWaterBlock::Update(void)
{
	// �u���b�N�̍X�V����
	CBlock::Update();

	// �v���C���[�̑����ɔg��𐶐����邽�߁A�v���C���[�̎擾������
	CPlayer* pPlayer = CGame::GetPlayer();

	// �ړ����Ă���Ƃ��ɐ������邽�߁A���͂��擾����
	InputData input = pPlayer->GatherInput();

	D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // �J�v�Z���R���C�_�[���S�ʒu

	// �J�v�Z����AABB�̓����蔻��
	bool isOverlap = CCollision::CheckCapsuleAABBCollision(GetPos(), GetModelSize(), GetSize(),
		pPos, pPlayer->GetRadius(), pPlayer->GetHeight());

	if (pPlayer && isOverlap && (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f))
	{
		m_counter++;

		// �v���C���[�̈ʒu
		D3DXVECTOR3 pos = pPlayer->GetPos();
		pos.y += 10.0f;

		if (m_counter >= 7)
		{
			// ���a�����߂ă����_���ʒu�ɃX�|�[��
			float radiusMax = 10.0f;

			// 0.0�`1.0 �̗���
			float r = (rand() % 10000) / 10000.0f;

			// ������������ċψ�ɕ��z������
			float radius = sqrtf(r) * radiusMax;

			float angle = ((rand() % 360) / 180.0f) * D3DX_PI;

			// �ʒu
			pos.x = pos.x + cosf(angle) * radius;
			pos.z = pos.z + sinf(angle) * radius;

			// �g��̐���
			CMeshCylinder::Create(pos, D3DXCOLOR(0.3f, 0.6f, 1.0f, 0.9f), 5.0f, 8.0f, 0.5f, 50, 0.03f);

			m_counter = 0;
		}
	}
	else
	{
		// �J�E���^�[�����Z�b�g
		m_counter = 0;
	}
}
