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
	m_move			= INIT_VEC3;						// �ړ���
	m_mtxWorld		= {};								// ���[���h�}�g���b�N�X
	m_nNumModel		= 0;								// ���f��(�p�[�c)�̑���
	m_pShadowS		= nullptr;							// �X�e���V���V���h�E�ւ̃|�C���^
	m_pDebug3D		= nullptr;							// 3D�f�o�b�O�\���ւ̃|�C���^
	m_pMotion		= nullptr;							// ���[�V�����ւ̃|�C���^
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

	pEnemy->SetPos(pos);
	pEnemy->SetRot(D3DXToRadian(rot));

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

	// �ŏ��̌���
	SetRot(D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f));

	// �J�v�Z���R���C�_�[�̐ݒ�
	CreatePhysics(CAPSULE_RADIUS, CAPSULE_HEIGHT);

	// �X�e���V���V���h�E�̐���
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x", GetPos());
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

	// �����̍X�V����
	UpdateRotation(0.09f);

	// �R���C�_�[�̈ʒu�X�V(�I�t�Z�b�g��ݒ�)
	UpdateCollider(D3DXVECTOR3(0, 45.0f, 0));// �����ɍ��킹��

	if (m_pShadowS != nullptr)
	{
		// �X�e���V���V���h�E�̈ʒu�ݒ�
		m_pShadowS->SetPosition(GetPos());
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