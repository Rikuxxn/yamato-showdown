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
#include "particle.h"
#include "guage.h"
#include "manager.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CPlayer::CPlayer()
{
	// �l�̃N���A
	memset(m_apModel, 0, sizeof(m_apModel));			// ���f��(�p�[�c)�ւ̃|�C���^
	m_mtxWorld			= {};							// ���[���h�}�g���b�N�X
	m_nNumModel			= 0;							// ���f��(�p�[�c)�̑���
	m_playerUse			= true;							// �g���Ă��邩�ǂ���
	m_pShadowS			= nullptr;						// �X�e���V���V���h�E�ւ̃|�C���^
	m_pMotion			= nullptr;						// ���[�V�����ւ̃|�C���^
	m_bIsMoving			= false;						// �ړ����̓t���O
	m_bOnGround			= false;						// �ڒn�t���O
	m_pDebug3D			= nullptr;						// 3D�f�o�b�O�\���ւ̃|�C���^
	m_particleTimer		= 0;							// �p�[�e�B�N���^�C�}�[
	m_pSwordModel		= nullptr;						// ���탂�f���̃|�C���^
	m_pWeaponCollider	= nullptr;						// ����̓����蔻��ւ̃|�C���^
	m_pTipModel			= nullptr;						// ����R���C�_�[�p���f��
	m_pBaseModel		= nullptr;						// ����R���C�_�[�p���f��
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

	pPlayer->SetPos(pos);
	pPlayer->SetRot(D3DXToRadian(rot));

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

		// ���O�� weapon ���܂܂�Ă����畐��p�[�c�ƔF��
		if (strstr(m_apModel[nCnt]->GetPath(), "weapon") != nullptr)
		{
			m_pSwordModel = m_apModel[nCnt];
		}
	}

	// �p�[�c������
	m_nNumModel = nNumModels;

	// ����R���C�_�[�̐���
	m_pWeaponCollider = make_unique<CWeaponCollider>();

#ifdef _DEBUG
	// ����R���C�_�[���f���̐���
	m_pTipModel = CObjectX::Create("data/MODELS/weapon_collider.x", m_pWeaponCollider->GetCurrentTipPos(), INIT_VEC3, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	m_pBaseModel = CObjectX::Create("data/MODELS/weapon_collider.x", m_pWeaponCollider->GetCurrentBasePos(), INIT_VEC3, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
#endif

	// �v���C���[���g���Ă���
	m_playerUse = true;

	// �ŏ��̌���
	SetRot(D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f));

	// �J�v�Z���R���C�_�[�̐ݒ�
	CreatePhysics(CAPSULE_RADIUS, CAPSULE_HEIGHT);

	// �X�e���V���V���h�E�̐���
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x",GetPos());
	m_pShadowS->SetStencilRef(1);// �ʂ̃X�e���V���o�b�t�@�̎Q�ƒl��ݒ�

	// �C���X�^���X�̃|�C���^��n��
	m_stateMachine.Start(this);

	// ������Ԃ̃X�e�[�g���Z�b�g
	m_stateMachine.ChangeState<CPlayer_StandState>();

	// HP�̐ݒ�
	SetHp(10.0f);

	// �Q�[�W�𐶐�
	SetGuages(D3DXVECTOR3(100.0f, 800.0f, 0.0f), 420.0f, 20.0f);

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
	m_bOnGround = OnGround(CManager::GetPhysicsWorld(), GetRigidBody(), 55.0f);

	// ����R���C�_�[�̍X�V
	m_pWeaponCollider->Update(m_pSwordModel);

#ifdef _DEBUG
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_1))
	{
		// �_���[�W����
		Damage(1.0f);
	}
	else if (pKeyboard->GetTrigger(DIK_2))
	{
		// �񕜏���
		Heal(1.0f);
	}

	// ����R���C�_�[�p���f���̈ʒu�X�V
	m_pTipModel->SetPos(m_pWeaponCollider->GetCurrentTipPos());
	m_pBaseModel->SetPos(m_pWeaponCollider->GetCurrentBasePos());

#endif

	// �X�e�[�g�}�V���X�V
	m_stateMachine.Update();

	// ���͔���̎擾
	InputData input = GatherInput();

	// �����̍X�V����
	UpdateRotation(0.09f);

	// �ړ����͂�����΃v���C���[��������͕�����
	if (!m_pMotion->IsAttacking() && (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f))
	{
		// Y�����������g�������̂ŖڕW�̌������擾
		D3DXVECTOR3 rotDest = GetRotDest();

		// Y����͕����Ɍ�����
		rotDest.y = atan2f(-input.moveDir.x, -input.moveDir.z);

		// �ڕW�̌����ɐݒ肷��
		SetRotDest(rotDest);
	}

	// �R���C�_�[�̈ʒu�X�V(�I�t�Z�b�g��ݒ�)
	UpdateCollider(D3DXVECTOR3(0, 35.0f, 0));// �����ɍ��킹��

	CModelEffect* pModelEffect = nullptr;

	//if (m_bIsMoving && m_bOnGround)
	//{
	//	m_particleTimer++;

	//	if (m_particleTimer >= DASH_PARTICLE_INTERVAL)
	//	{
	//		m_particleTimer = 0;

	//		// �����_���Ȋp�x�ŉ��ɍL����
	//		float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
	//		float speed = (rand() % 150) / 300.0f + 0.2f;

	//		// �ړ���
	//		D3DXVECTOR3 move;
	//		move.x = cosf(angle) * speed;
	//		move.z = sinf(angle) * speed;
	//		move.y = (rand() % 80) / 50.0f + 0.05f; // �������������

	//		// ����
	//		D3DXVECTOR3 rot;
	//		rot.x = ((rand() % 360) / 180.0f) * D3DX_PI;
	//		rot.y = ((rand() % 360) / 180.0f) * D3DX_PI;
	//		rot.z = ((rand() % 360) / 180.0f) * D3DX_PI;

	//		// ���f���G�t�F�N�g�̐���
	//		pModelEffect = CModelEffect::Create("data/MODELS/effectModel_step.x", GetPos(), rot,
	//			move, D3DXVECTOR3(0.3f, 0.3f, 0.3f), 180, 0.01f, 0.008f);
	//	}
	//}
	//else
	//{
	//	m_particleTimer = 0; // ��~���̓��Z�b�g
	//}

	if (m_pShadowS != nullptr)
	{
		// �X�e���V���V���h�E�̈ʒu�ݒ�
		m_pShadowS->SetPosition(GetPos());
	}

	// ���[�V�����̍X�V����
	m_pMotion->Update(m_apModel, m_nNumModel);
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
	D3DXMatrixScaling(&mtxSize, GetSize().x, GetSize().y, GetSize().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, GetRot().y, GetRot().x, GetRot().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, GetPos().x, GetPos().y, GetPos().z);
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

	btRigidBody* pRigid = GetRigidBody();
	btCollisionShape* pShape = GetCollisionShape();

	// �J�v�Z���R���C�_�[�̕`��
	if (pRigid && pShape)
	{
		btTransform transform;
		pRigid->getMotionState()->getWorldTransform(transform);

		m_pDebug3D->DrawCapsuleCollider((btCapsuleShape*)pShape, transform, D3DXCOLOR(1, 1, 1, 1));
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
// �v���C���[�̑O���x�N�g���擾
//=============================================================================
D3DXVECTOR3 CPlayer::GetForward(void)
{
	// �v���C���[�̉�]�p�x�iY���j����O���x�N�g�����v�Z
	float yaw = GetRot().y;

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

	if (CGame::GetEnemy()->IsDead())
	{
		return input;
	}

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

