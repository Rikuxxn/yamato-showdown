//=============================================================================
//
// �L�����N�^�[�}�l�[�W���[���� [charactermanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "charactermanager.h"


//=============================================================================
// �R���X�g���N�^
//=============================================================================
CCharacter::CCharacter(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_fHp           = 0.0f;                         // HP
	m_fMaxHp        = 0.0f;                         // HP�ő��
	m_pHpGuage      = nullptr;		                // �΃Q�[�W
	m_pBackGuage    = nullptr;		                // �ԃQ�[�W�i�x��ĒǏ]�j
	m_pFrame        = nullptr;			            // �g
	m_pos           = INIT_VEC3;					// �ʒu
	m_rot           = INIT_VEC3;					// ����
	m_rotDest       = INIT_VEC3;					// ����
	m_size          = D3DXVECTOR3(1.0f, 1.0f, 1.0f);// �T�C�Y
	m_pRigidBody    = nullptr;						// ���̂ւ̃|�C���^
	m_pShape        = nullptr;						// �����蔻��̌`�ւ̃|�C���^
	m_radius        = 0.0f;							// �J�v�Z���R���C�_�[�̔��a
	m_height        = 0.0f;							// �J�v�Z���R���C�_�[�̍���
	m_colliderPos   = INIT_VEC3;					// �R���C�_�[�̈ʒu

}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CCharacter::~CCharacter()
{
	// �Ȃ�
}
//=============================================================================
// �����蔻��̐�������
//=============================================================================
void CCharacter::CreatePhysics(float radius, float height)
{
    //*********************************************************************
    // Bullet Physics �J�v�Z���R���C�_�[�̐ݒ�
    //*********************************************************************

    m_radius = radius;
    m_height = height;

    // �R���C�_�[�𐶐�(�J�v�Z��)
    m_pShape = new btCapsuleShape(m_radius, m_height);

    // �R���C�_�[���S = ���� + �I�t�Z�b�g
    m_colliderPos = GetPos() + D3DXVECTOR3(0, 20.0f, 0);

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
}
//=============================================================================
// Physics�̔j��
//=============================================================================
void CCharacter::ReleasePhysics(void)
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
// �����蔻��̈ʒu�X�V
//=============================================================================
void CCharacter::UpdateCollider(D3DXVECTOR3 offset)
{
    // ���݈ʒu�𕨗����[���h����擾���� m_pos �ɔ��f
    btTransform trans;
    m_pRigidBody->getMotionState()->getWorldTransform(trans);
    btVector3 pos = trans.getOrigin();
    m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
    m_pos = m_colliderPos - offset; // �����ւ̃I�t�Z�b�g
}
//=============================================================================
// ������ԏ���
//=============================================================================
void CCharacter::UpdateRotation(float fInterpolationSpeed)
{
    // Y���̍�����180�x�𒴂���ꍇ�͕␳
    if (m_rotDest.y - m_rot.y > D3DX_PI)
    {
        m_rot.y += D3DX_PI * 2.0f;
    }
    else if (m_rotDest.y - m_rot.y < -D3DX_PI)
    {
        m_rot.y -= D3DX_PI * 2.0f;
    }

    // ��Ԃ��Ċ��炩�ɉ�]
    m_rot.y += (m_rotDest.y - m_rot.y) * fInterpolationSpeed;
}
