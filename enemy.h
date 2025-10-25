//=============================================================================
//
// �G�̏��� [enemy.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "shadowS.h"
#include "effect.h"
#include "state.h"
#include "manager.h"
#include "debugproc3D.h"
#include "guage.h"
#include "charactermanager.h"

//*****************************************************************************
// �G�N���X
//*****************************************************************************
class CEnemy : public CCharacter
{
public:
	CEnemy();
	~CEnemy();

	// �G���[�V�����̎��
	typedef enum
	{
		NEUTRAL = 0,		// �ҋ@
		MOVE,				// �ړ�
		ATTACK_01,			// �U��1
		MAX
	}ENEMY_MOTION;

	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	CMotion* GetMotion(void) { return m_pMotion; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; };
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	btScalar GetRadius(void) const { return m_radius; }
	btScalar GetHeight(void) const { return m_height; }
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾

	void ReleasePhysics(void);														// Physics�j���p

private:
	static constexpr int MAX_PARTS = 32;	// �ő�p�[�c��
	static constexpr float CAPSULE_RADIUS = 14.0f;					// �J�v�Z���R���C�_�[�̔��a
	static constexpr float CAPSULE_HEIGHT = 60.0f;					// �J�v�Z���R���C�_�[�̍���
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DXVECTOR3 m_rot;						// ����
	D3DXVECTOR3 m_rotDest;					// ����
	D3DXVECTOR3 m_move;						// �ړ���
	D3DXVECTOR3 m_size;						// �T�C�Y
	D3DXVECTOR3 m_colliderPos;				// �J�v�Z���R���C�_�[�̈ʒu
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	CModel* m_apModel[MAX_PARTS];			// ���f��(�p�[�c)�ւ̃|�C���^
	CShadowS* m_pShadowS;					// �X�e���V���V���h�E�ւ̃|�C���^
	CDebugProc3D* m_pDebug3D;				// 3D�f�o�b�O�\���ւ̃|�C���^
	CMotion* m_pMotion;						// ���[�V�����ւ̃|�C���^
	btRigidBody* m_pRigidBody;				// ���̂ւ̃|�C���^
	btCollisionShape* m_pShape;				// �����蔻��̌`�ւ̃|�C���^
	btScalar m_radius;						// �J�v�Z���R���C�_�[�̔��a
	btScalar m_height;						// �J�v�Z���R���C�_�[�̍���
	int m_nNumModel;						// ���f��(�p�[�c)�̑���

	// �X�e�[�g���Ǘ�����N���X�̃C���X�^���X
	StateMachine<CEnemy> m_stateMachine;

};


//*****************************************************************************
// �G�̑ҋ@���
//*****************************************************************************
class CEnemy_StandState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// �ҋ@���[�V����
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::NEUTRAL, 10);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		//// �U�����1�ֈڍs
		//m_pMachine->ChangeState<CEnemy_AttackState1>();

	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

////*****************************************************************************
//// �G�̈ړ����
////*****************************************************************************
//class CEnemy_MoveState :public StateBase<CEnemy>
//{
//public:
//
//	void OnStart(CEnemy* pEnemy)override
//	{
//		// �ړ����[�V����
//		pEnemy->GetMotion()->StartBlendMotion(CEnemy::MOVE, 10);
//	}
//
//	void OnUpdate(CEnemy* pEnemy)override
//	{
//
//
//	}
//
//	void OnExit(CEnemy* /*pEnemy*/)override
//	{
//
//	}
//
//private:
//
//};
//
////*****************************************************************************
//// �G�̂̍U�����1
////*****************************************************************************
//class CEnemy_AttackState1 :public StateBase<CEnemy>
//{
//public:
//
//	void OnStart(CEnemy* pEnemy)override
//	{
//		// �U�����[�V����
//		pEnemy->GetMotion()->StartBlendMotion(CEnemy::ATTACK_01, 10);
//	}
//
//	void OnUpdate(CEnemy* pEnemy)override
//	{
//
//	}
//
//	void OnExit(CEnemy* /*pEnemy*/)override
//	{
//
//	}
//
//private:
//
//};

#endif

