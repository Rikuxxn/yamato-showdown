//=============================================================================
//
// �v���C���[���� [player.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PLAYER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "debugproc3D.h"
#include "block.h"
#include "shadowS.h"
#include "effect.h"
#include "state.h"
#include "manager.h"
#include "charactermanager.h"

// �O���錾
class CPlayer_StandState;
class CPlayer_MoveState;
class CPlayer_AttackState1;

// ���̓f�[�^�\����
struct InputData
{
	D3DXVECTOR3 moveDir;      // �O��ړ��x�N�g��
	bool attack;              // �U������
};

//*****************************************************************************
// �v���C���[�N���X
//*****************************************************************************
class CPlayer : public CCharacter
{
public:
	CPlayer();
	~CPlayer();

	static constexpr float PLAYER_SPEED = 200.0f;				// �ړ��X�s�[�h

	// �v���C���[���[�V�����̎��
	typedef enum
	{
		NEUTRAL = 0,		// �ҋ@
		MOVE,				// �ړ�
		ATTACK_01,			// �U��1
		MAX
	}PLAYER_MOTION;

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// flagment�֐�
	//*****************************************************************************
	bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	CMotion* GetMotion(void) { return m_pMotion; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	bool GetPlayerUse(void) const { return m_playerUse; }
	bool GetOnGround(void) { return m_bOnGround; }
	bool GetIsMoving(void) const { return m_bIsMoving; }
	D3DXVECTOR3 GetForward(void);
	InputData GatherInput(void);
	CBlock* FindFrontBlockByRaycast(float rayLength);

	// �X�e�[�g�p�Ƀt���O�X�V
	void UpdateMovementFlags(const D3DXVECTOR3& moveDir)
	{
		m_bIsMoving = (moveDir.x != 0.0f || moveDir.z != 0.0f);
	}

private:
	static constexpr int MAX_PARTS			= 32;		// �ő�p�[�c��
	static constexpr float MAX_GRAVITY		= -0.26f;	// �d�͉����x
	static constexpr float CAPSULE_RADIUS	= 10.5f;	// �J�v�Z���R���C�_�[�̔��a
	static constexpr float CAPSULE_HEIGHT	= 45.5f;	// �J�v�Z���R���C�_�[�̍���

	D3DXVECTOR3 m_move;					// �ړ���
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X
	CModel* m_apModel[MAX_PARTS];		// ���f��(�p�[�c)�ւ̃|�C���^
	CShadowS* m_pShadowS;				// �X�e���V���V���h�E�ւ̃|�C���^
	CMotion* m_pMotion;					// ���[�V�����ւ̃|�C���^
	CDebugProc3D* m_pDebug3D;			// 3D�f�o�b�O�\���ւ̃|�C���^
	int m_nNumModel;					// ���f��(�p�[�c)�̑���
	bool m_playerUse;					// �g���Ă��邩�ǂ���
	bool m_bIsMoving;					// �ړ����̓t���O
	bool m_bOnGround;					// �ڒn�t���O
	int m_particleTimer;				// �p�[�e�B�N���^�C�}�[
	static constexpr int DASH_PARTICLE_INTERVAL = 10; // �p�[�e�B�N�������Ԋu�i�t���[�����j

	// �X�e�[�g���Ǘ�����N���X�̃C���X�^���X
	StateMachine<CPlayer> m_stateMachine;
};


//*****************************************************************************
// �v���C���[�̑ҋ@���
//*****************************************************************************
class CPlayer_StandState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{		
		// �ҋ@���[�V����
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::NEUTRAL, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// ���͂��擾
		InputData input = pPlayer->GatherInput();

		// �U�����͂�����΍U���X�e�[�g1�Ɉڍs
		if (input.attack && !pPlayer->GetMotion()->IsAttacking())
		{
			// �U�����1�ֈڍs
			m_pMachine->ChangeState<CPlayer_AttackState1>();
			return;
		}

		// �ړ����͂�����Έړ��X�e�[�g�Ɉڍs
		if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
		{
			// �ړ���Ԃֈڍs
			m_pMachine->ChangeState<CPlayer_MoveState>();
		}

		D3DXVECTOR3 move = pPlayer->GetMove();

		move *= 0.85f; // ������
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// �ړ��ʂ�ݒ�
		pPlayer->SetMove(move);

		// ���W�b�h�{�f�B�ɔ��f
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

//*****************************************************************************
// �v���C���[�̈ړ����
//*****************************************************************************
class CPlayer_MoveState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		if (!pPlayer->GetMotion()->IsAttacking())
		{
			// �ړ����[�V����
			pPlayer->GetMotion()->StartBlendMotion(CPlayer::MOVE, 10);
		}
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// ���͎擾
		InputData input = pPlayer->GatherInput();

		// �t���O�X�V
		pPlayer->UpdateMovementFlags(input.moveDir);

		// �U�����͂�����΍U���X�e�[�g1�Ɉڍs
		if (input.attack)
		{
			// �U�����1
			m_pMachine->ChangeState<CPlayer_AttackState1>();
			return;
		}

		// �ڕW���x�v�Z
		float moveSpeed = CPlayer::PLAYER_SPEED;

		D3DXVECTOR3 targetMove = input.moveDir;

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);
			moveSpeed = CPlayer::PLAYER_SPEED;

			targetMove *= moveSpeed;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// ���ݑ��x�Ƃ̕�ԁi�C�[�W���O�j
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// ��Ԍ�̑��x���v���C���[�ɃZ�b�g
		pPlayer->SetMove(currentMove);

		// �����G���W���ɃZ�b�g
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		// �ړ����Ă��Ȃ���Αҋ@�X�e�[�g�ɖ߂�
		if (!pPlayer->GetIsMoving() && !pPlayer->GetMotion()->IsAttacking())
		{
			// �ҋ@���
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

//*****************************************************************************
// �v���C���[�̍U�����1
//*****************************************************************************
class CPlayer_AttackState1 :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// �U�����[�V����
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::ATTACK_01, 10);

		// �����Ă�������ɃX���C�h�����邽�߁A�v���C���[�̑O���x�N�g�����擾
		D3DXVECTOR3 dir = pPlayer->GetForward();

		// ���K��
		D3DXVec3Normalize(&dir, &dir);

		float dashPower = 800.0f;// �X���C�h�p���[

		D3DXVECTOR3 move = dir * dashPower;

		// ���݂̈ړ��ʂɏ㏑��
		pPlayer->SetMove(move);

		// �������x�ɂ������f
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// ���͎擾
		InputData input = pPlayer->GatherInput();

		// �t���O�X�V
		pPlayer->UpdateMovementFlags(input.moveDir);

		D3DXVECTOR3 move = pPlayer->GetMove();

		move *= 0.85f; // ������
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// �ړ��ʂ�ݒ�
		pPlayer->SetMove(move);

		// ���W�b�h�{�f�B�ɔ��f
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (pPlayer->GetMotion()->IsCurrentMotionEnd(CPlayer::ATTACK_01))
		{// �U�����[�V�������I����Ă�����
			// �ҋ@���
			pPlayer->GetMotion()->SetMotion(CPlayer::NEUTRAL);
			m_pMachine->ChangeState<CPlayer_StandState>();

			return;
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

#endif
