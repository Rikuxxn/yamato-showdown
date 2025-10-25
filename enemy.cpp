//=============================================================================
//
// �G�̏��� [enemy.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "enemy.h"
#include "guage.h"


// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CEnemy::CEnemy()
{
	// �l�̃N���A
	memset(m_apModel, 0, sizeof(m_apModel));			// ���f��(�p�[�c)�ւ̃|�C���^
	m_pos			= INIT_VEC3;						// �ʒu
	m_rot			= INIT_VEC3;						// ����
	m_rotDest		= INIT_VEC3;						// ����
	m_move			= INIT_VEC3;						// �ړ���
	m_size			= D3DXVECTOR3(1.0f, 1.0f, 1.0f);	//�T�C�Y
	m_mtxWorld		= {};								// ���[���h�}�g���b�N�X
	m_nNumModel		= 0;								// ���f��(�p�[�c)�̑���
	m_pShadowS		= nullptr;							// �X�e���V���V���h�E�ւ̃|�C���^
	m_pDebug3D		= nullptr;							// 3D�f�o�b�O�\���ւ̃|�C���^
	m_pMotion		= nullptr;							// ���[�V�����ւ̃|�C���^
	m_pRigidBody	= nullptr;							// ���̂ւ̃|�C���^
	m_pShape		= nullptr;							// �����蔻��̌`�ւ̃|�C���^
	m_radius		= 0.0f;								// �J�v�Z���R���C�_�[�̔��a
	m_height		= 0.0f;								// �J�v�Z���R���C�_�[�̍���
	m_colliderPos	= INIT_VEC3;						// �R���C�_�[�̈ʒu
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CEnemy::~CEnemy()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CEnemy* pEnemy = new CEnemy;

	pEnemy->m_pos = pos;
	pEnemy->m_rot = D3DXToRadian(rot);

	// ����������
	pEnemy->Init();

	return pEnemy;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CEnemy::Init(void)
{
	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// �p�[�c�̓ǂݍ���
	m_pMotion = CMotion::Load("data/motion_enemy.txt", pModels, nNumModels, MAX);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// �I�t�Z�b�g�l��
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// �p�[�c������
	m_nNumModel = nNumModels;

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
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x", m_pos);
	m_pShadowS->SetStencilRef(2);// �ʂ̃X�e���V���o�b�t�@�̎Q�ƒl��ݒ�

	// �C���X�^���X�̃|�C���^��n��
	m_stateMachine.Start(this);

	// ������Ԃ̃X�e�[�g���Z�b�g
	m_stateMachine.ChangeState<CEnemy_StandState>();

	// HP�̐ݒ�
	SetHp(100.0f);

	// �Q�[�W�𐶐�
	SetGuages(D3DXVECTOR3(600.0f, 100.0f, 0.0f), 620.0f, 10.0f);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CEnemy::Uninit(void)
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
void CEnemy::Update(void)
{

#ifdef _DEBUG
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_3))
	{
		// �_���[�W����
		Damage(1.0f);
	}
	else if (pKeyboard->GetTrigger(DIK_4))
	{
		// �񕜏���
		Heal(1.0);
	}

#endif

	// �X�e�[�g�}�V���X�V
	m_stateMachine.Update();

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

	// ���݈ʒu�𕨗����[���h����擾���� m_pos �ɔ��f
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 45.0f, 0); // �����ւ̃I�t�Z�b�g

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
void CEnemy::Draw(void)
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
// Physics�̔j��
//=============================================================================
void CEnemy::ReleasePhysics(void)
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
