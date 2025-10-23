//=============================================================================
//
// �v���C���[���� [player.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "player.h"
#include "texture.h"
#include "model.h"
#include "particle.h"
#include "game.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	memset(m_apModel, 0, sizeof(m_apModel));			// ���f��(�p�[�c)�ւ̃|�C���^
	m_pos				= INIT_VEC3;					// �ʒu
	m_rot				= INIT_VEC3;					// ����
	m_rotDest			= INIT_VEC3;					// ����
	m_move				= INIT_VEC3;					// �ړ���
	m_targetMove		= INIT_VEC3;					// �ڕW���x
	m_currentMove		= INIT_VEC3;					// ���ۂ̈ړ����x
	m_size				= D3DXVECTOR3(1.0f, 1.0f, 1.0f);// �T�C�Y
	m_mtxWorld			= {};							// ���[���h�}�g���b�N�X
	m_nNumModel			= 0;							// ���f��(�p�[�c)�̑���
	m_playerUse			= true;							// �g���Ă��邩�ǂ���
	m_pShadowS			= nullptr;						// �X�e���V���V���h�E�ւ̃|�C���^
	m_pMotion			= nullptr;						// ���[�V�����ւ̃|�C���^
	m_bIsMoving			= false;						// �ړ����̓t���O
	m_pRigidBody		= nullptr;						// ���̂ւ̃|�C���^
	m_bOnGround			= false;						// �ڒn�t���O
	m_pShape			= nullptr;						// �����蔻��̌`�ւ̃|�C���^
	m_pDebug3D			= nullptr;						// 3D�f�o�b�O�\���ւ̃|�C���^
	m_radius			= 0.0f;							// �J�v�Z���R���C�_�[�̔��a
	m_height			= 0.0f;							// �J�v�Z���R���C�_�[�̍���
	m_colliderPos		= INIT_VEC3;					// �R���C�_�[�̈ʒu
	m_pCarryingBlock	= nullptr;						// �^��ł���u���b�N
	m_particleTimer		= 0;							// �p�[�e�B�N���^�C�}�[

}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CPlayer::~CPlayer()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = new CPlayer;

	pPlayer->m_pos = pos;
	pPlayer->m_rot = D3DXToRadian(rot);

	// ����������
	pPlayer->Init();

	return pPlayer;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CPlayer::Init(void)
{
	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// �p�[�c�̓ǂݍ���
	m_pMotion = CMotion::Load("data/motion.txt", pModels, nNumModels, MAX);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// �I�t�Z�b�g�l��
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// �p�[�c������
	m_nNumModel = nNumModels;

	// �v���C���[���g���Ă���
	m_playerUse = true;

	// �ϐ��̏�����
	m_rot = D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f);

	//*********************************************************************
	// Bullet Physics �J�v�Z���R���C�_�[�̐ݒ�
	//*********************************************************************

	m_radius = CAPSULE_RADIUS;
	m_height = CAPSULE_HEIGHT;

	m_pShape = new btCapsuleShape(m_radius, m_height);

	// �R���C�_�[���S = ���� + �I�t�Z�b�g
	m_colliderPos = m_pos + D3DXVECTOR3(0, 20.0f, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(m_colliderPos.x, m_colliderPos.y, m_colliderPos.z));

	// ���ʂ�ݒ�
	btScalar mass = 2.0f;
	btVector3 inertia(0, 0, 0);  // ����

	m_pShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);

	m_pRigidBody->setAngularFactor(btVector3(0, 0, 0));
	m_pRigidBody->setFriction(0.0f);// ���C
	m_pRigidBody->setRollingFriction(0.0f);// �]���薀�C

	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// �X���[�v��Ԃɂ��Ȃ�

	// �������[���h�ɒǉ�
	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != nullptr)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}

	// �X�e���V���V���h�E�̐���
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x",m_pos);

	// �C���X�^���X�̃|�C���^��n��
	m_stateMachine.Start(this);

	// ������Ԃ̃X�e�[�g���Z�b�g
	m_stateMachine.ChangeState<CPlayer_StandState>();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CPlayer::Uninit(void)
{
	ReleasePhysics();

	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			m_apModel[nCnt]->Uninit();
			delete m_apModel[nCnt];
			m_apModel[nCnt] = nullptr;
		}
	}

	if (m_pMotion != nullptr)
	{
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CPlayer::Update(void)
{
	// �J�����̎擾
	CCamera* pCamera = CManager::GetCamera();

	// �J�����̊p�x�̎擾
	D3DXVECTOR3 CamRot = pCamera->GetRot();

	// �ڒn����
	m_bOnGround = OnGround(CManager::GetPhysicsWorld(), m_pRigidBody, 55.0f);

	// �X�e�[�g�}�V���X�V
	m_stateMachine.Update();

	// ���͔���̎擾
	InputData input = GatherInput();

	// �����̐��K��
	if (m_rotDest.y - m_rot.y > D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
	else if (m_rotDest.y - m_rot.y < -D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}

	m_rot.y += (m_rotDest.y - m_rot.y) * 0.09f;

	// �ړ����͂�����΃v���C���[��������͕�����
	if (!m_pMotion->IsAttacking() && (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f))
	{
		m_rotDest.y = atan2f(-input.moveDir.x,-input.moveDir.z);
	}

	// ���݈ʒu�𕨗����[���h����擾���� m_pos �ɔ��f
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 35.0f, 0); // �����ւ̃I�t�Z�b�g

	CModelEffect* pModelEffect = nullptr;

	if (m_bIsMoving && m_bOnGround)
	{
		m_particleTimer++;

		if (m_particleTimer >= DASH_PARTICLE_INTERVAL)
		{
			m_particleTimer = 0;

			// �����_���Ȋp�x�ŉ��ɍL����
			float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
			float speed = (rand() % 150) / 300.0f + 0.2f;

			// �ړ���
			D3DXVECTOR3 move;
			move.x = cosf(angle) * speed;
			move.z = sinf(angle) * speed;
			move.y = (rand() % 80) / 50.0f + 0.05f; // �������������

			// ����
			D3DXVECTOR3 rot;
			rot.x = ((rand() % 360) / 180.0f) * D3DX_PI;
			rot.y = ((rand() % 360) / 180.0f) * D3DX_PI;
			rot.z = ((rand() % 360) / 180.0f) * D3DX_PI;

			// ���f���G�t�F�N�g�̐���
			pModelEffect = CModelEffect::Create("data/MODELS/effectModel_step.x", m_pos, rot,
				move, D3DXVECTOR3(0.3f, 0.3f, 0.3f), 180, 0.01f, 0.008f);
		}
	}
	else
	{
		m_particleTimer = 0; // ��~���̓��Z�b�g
	}

	if (m_pShadowS != nullptr)
	{
		// �X�e���V���V���h�E�̈ʒu�ݒ�
		m_pShadowS->SetPosition(m_pos);
	}

	int nNumModels = 16;

	// ���[�V�����̍X�V����
	m_pMotion->Update(m_apModel, nNumModels);
}
//=============================================================================
// �`�揈��
//=============================================================================
void CPlayer::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �T�C�Y�𔽉f
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntMat = 0; nCntMat < m_nNumModel; nCntMat++)
	{
		// ���f��(�p�[�c)�̕`��
		if (m_apModel[nCntMat])
		{
			m_apModel[nCntMat]->Draw();
		}
	}

#ifdef _DEBUG

	// �J�v�Z���R���C�_�[�̕`��
	if (m_pRigidBody && m_pShape)
	{
		btTransform transform;
		m_pRigidBody->getMotionState()->getWorldTransform(transform);

		m_pDebug3D->DrawCapsuleCollider((btCapsuleShape*)m_pShape, transform, D3DXCOLOR(1, 1, 1, 1));
	}

#endif

}
//=============================================================================
// ���C�ɂ��ڐG����
//=============================================================================
bool CPlayer::OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength)
{
	btTransform trans;
	playerBody->getMotionState()->getWorldTransform(trans);

	btVector3 start = trans.getOrigin();
	btVector3 end = start - btVector3(0, rayLength, 0);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}
	};

	RayResultCallback rayCallback(start, end);
	world->rayTest(start, end, rayCallback);

	if (rayCallback.hasHit())
	{// �ڐG
		// �q�b�g�����ʂ̖@��
		btVector3 normal = rayCallback.m_hitNormalWorld.normalized();

		// ������x�N�g��
		btVector3 up(0, 1, 0);

		// �@���Ə�����̓��ρi= cos�Ɓj
		float slopeDot = normal.dot(up);

		// cos�� ���������قǋ}�Ζ�
		// 50�x�܂ł�n�ʂƂ��Ĉ���
		float slopeLimit = cosf(D3DXToRadian(50.0f));

		if (slopeDot >= slopeLimit)
		{
			return true; // �ڒn
		}
		else
		{
			return false; // �}�ΖʂȂ̂Őڒn�������Ȃ�
		}
	}

	return false;
}
//=============================================================================
// Physics�̔j��
//=============================================================================
void CPlayer::ReleasePhysics(void)
{
	if (m_pRigidBody)
	{
		btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();
		if (pWorld) pWorld->removeRigidBody(m_pRigidBody);
		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = nullptr;
	}

	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = nullptr;
	}
}
//=============================================================================
// �v���C���[�̑O���x�N�g���擾
//=============================================================================
D3DXVECTOR3 CPlayer::GetForward(void) const
{
	// �v���C���[�̉�]�p�x�iY���j����O���x�N�g�����v�Z
	float yaw = m_rot.y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// ���K������
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// �v���C���[�̑O�����C���菈��
//=============================================================================
CBlock* CPlayer::FindFrontBlockByRaycast(float rayLength)
{
	btDiscreteDynamicsWorld* world = CManager::GetPhysicsWorld();

	if (!world)
	{
		return nullptr;
	}

	D3DXVECTOR3 from = GetPos() + D3DXVECTOR3(0.0f,20.0f,0.0f);
	D3DXVECTOR3 to = from + GetForward() * rayLength;

	btVector3 btFrom(from.x, from.y, from.z);
	btVector3 btTo(to.x, to.y, to.z);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			// �u���b�N�ȊO�𖳎�����ꍇ�͂����Ńt�B���^�[���Ă�����
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}
	};

	RayResultCallback rayCallback(btFrom, btTo);
	world->rayTest(btFrom, btTo, rayCallback);

	if (rayCallback.hasHit())
	{
		void* userPtr = rayCallback.m_collisionObject->getUserPointer();

		if (userPtr)
		{
			CBlock* pBlock = static_cast<CBlock*>(userPtr);

			if (pBlock->IsDynamicBlock())
			{
				return pBlock; // ���I�ȃu���b�N���O���ɂ�����
			}
		}
	}

	return nullptr;
}
//=============================================================================
// ���͔���擾�֐�
//=============================================================================
InputData CPlayer::GatherInput(void)
{
	InputData input{};
	input.moveDir = D3DXVECTOR3(0, 0, 0);
	input.attack = false;

	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();	// �L�[�{�[�h�̎擾
	CInputMouse* pMouse = CManager::GetInputMouse();			// �}�E�X�̎擾
	CInputJoypad* pJoypad = CManager::GetInputJoypad();			// �W���C�p�b�h�̎擾
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();		// �X�e�B�b�N�̎擾
	CCamera* pCamera = CManager::GetCamera();					// �J�����̎擾
	D3DXVECTOR3 CamRot = pCamera->GetRot();						// �J�����p�x�̎擾

	// ---------------------------
	// �U������
	// ---------------------------
	if (pMouse->GetTrigger(0) || pJoypad->GetTrigger(pJoypad->JOYKEY_X))
	{
		input.attack = true;
	}

	// ---------------------------
	// �U�����͈ړ����͖�����
	// ---------------------------
	if (m_pMotion->IsAttacking())
	{
		return input;
	}

	// ---------------------------
	// �Q�[���p�b�h����
	// ---------------------------
	if (pJoypad->GetStick() && pStick)
	{
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;
		float magnitude = sqrtf(stickX * stickX + stickY * stickY);
		const float DEADZONE = 10922.0f;

		if (magnitude >= DEADZONE)
		{
			stickX /= magnitude;
			stickY /= magnitude;
			float normMag = std::min((magnitude - DEADZONE) / (32767.0f - DEADZONE), 1.0f);
			stickX *= normMag;
			stickY *= normMag;

			D3DXVECTOR3 dir;
			float yaw = CamRot.y;

			dir.x = -(stickX * cosf(yaw) + stickY * sinf(yaw));
			dir.z = stickX * sinf(-yaw) + stickY * cosf(yaw);
			dir.z = -dir.z;

			input.moveDir += D3DXVECTOR3(dir.x, 0, dir.z);
		}
	}

	// ---------------------------
	// �L�[�{�[�h����
	// ---------------------------
	if (pKeyboard->GetPress(DIK_W))
	{
		input.moveDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_S))
	{
		input.moveDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_A))
	{
		input.moveDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_D))
	{
		input.moveDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// ���K��
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		D3DXVec3Normalize(&input.moveDir, &input.moveDir);
	}

	return input;
}

