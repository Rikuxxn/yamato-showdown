//=============================================================================
//
// �u���b�N���� [block.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"
#include "manager.h"
#include "particle.h"
#include "game.h"
#include "result.h"
#include "blocklist.h"
#include "grass.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::unordered_map<CBlock::TYPE, BlockCreateFunc> CBlock::m_BlockFactoryMap = {};

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CBlock::CBlock(int nPriority) : CObjectX(nPriority)
{
	// �l�̃N���A
	m_col			 = INIT_XCOL;				// �F
	m_baseCol		 = INIT_XCOL;				// �x�[�X�̐F
	m_bSelected		 = false;					// �I���t���O
	m_pDebug3D		 = nullptr;					// 3D�f�o�b�O�\���ւ̃|�C���^
	m_prevSize		 = INIT_VEC3;				// �O��̃T�C�Y
	m_colliderSize	 = INIT_VEC3;				// �R���C�_�[�T�C�Y
	m_colliderOffset = INIT_VEC3;				// �R���C�_�[�̃I�t�Z�b�g
	m_isEditMode	 = false;					// �ҏW�����ǂ���
	m_isDead		 = false;					// �폜�\��t���O
}
//=============================================================================
// ��������
//=============================================================================
CBlock* CBlock::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size, TYPE type)
{
	if (m_BlockFactoryMap.empty())
	{
		// �t�@�N�g���[
		InitFactory();
	}

	CBlock* pBlock = nullptr;

	auto it = m_BlockFactoryMap.find(type);
	if (it != m_BlockFactoryMap.end())
	{
		pBlock = it->second();
	}
	else
	{
		pBlock = new CBlock(); // �f�t�H���g���N���X
	}

	if (!pBlock)
	{
		return nullptr;
	}

	pBlock->SetPos(pos);
	pBlock->SetRot(rot);
	pBlock->SetSize(size);
	pBlock->SetType(type);
	pBlock->SetPath(pFilepath);

	pBlock->Init();
	pBlock->CreatePhysicsFromScale(size);

	return pBlock;
}
//=============================================================================
// �t�@�N�g���[
//=============================================================================
void CBlock::InitFactory(void)
{
	// ���X�g����ɂ���
	m_BlockFactoryMap.clear();

	m_BlockFactoryMap[CBlock::TYPE_TORCH_01]		= []() -> CBlock* { return new CTorchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_GRASS]			= []() -> CBlock* { return new CGrassBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_TORCH_01]		= []() -> CBlock* { return new CTorchBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_WATER]			= []() -> CBlock* { return new CWaterBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_WOODBOX]			= []() -> CBlock* { return new CWoodBoxBlock(); };
	m_BlockFactoryMap[CBlock::TYPE_SEESAW]			= []() -> CBlock* { return new CSeesawBlock(); };
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CBlock::Init(void)
{
	// �I�u�W�F�N�gX�̏���������
	CObjectX::Init();

	// �}�e���A���F���u���b�N�̐F�ɐݒ�
	m_col = GetMaterialColor();
	m_col = m_baseCol;              // ���݂̐F�ɂ���x����Ă���

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CBlock::Uninit(void)
{
	ReleasePhysics();

	// �I�u�W�F�N�gX�̏I������
	CObjectX::Uninit();
}
//=============================================================================
// �X�V����
//=============================================================================
void CBlock::Update(void)
{
	// �ÓI�u���b�N�� Transform ���蓮�ōX�V
	if (!IsDynamicBlock() || IsEditMode())
	{
		D3DXVECTOR3 Pos = GetPos() + m_colliderOffset;
		D3DXVECTOR3 Rot = GetRot();

		btTransform trans;
		trans.setIdentity();

		D3DXMATRIX matRot;
		D3DXMatrixRotationYawPitchRoll(&matRot, Rot.y, Rot.x, Rot.z);

		D3DXQUATERNION dq;
		D3DXQuaternionRotationMatrix(&dq, &matRot);

		btQuaternion q(dq.x, dq.y, dq.z, dq.w);
		q.normalize();// ���K��

		trans.setOrigin(btVector3(Pos.x, Pos.y, Pos.z));
		trans.setRotation(q);

		if (m_pRigidBody && m_pRigidBody->getMotionState())
		{
			m_pRigidBody->setWorldTransform(trans);
			m_pRigidBody->getMotionState()->setWorldTransform(trans);
		}
	}
	else
	{
		// ���I�u���b�N�͕����̕ϊ����擾���Ĕ��f

		if (!m_pRigidBody || !m_pRigidBody->getMotionState())
		{
			return;
		}

		btTransform trans;

		m_pRigidBody->getMotionState()->getWorldTransform(trans);

		btVector3 pos = trans.getOrigin();

		// �ʒu�Z�b�g�i�I�t�Z�b�g���������j
		D3DXVECTOR3 newPos(pos.x(), pos.y(), pos.z());
		SetPos(newPos - m_colliderOffset);

		btQuaternion rot = m_pRigidBody->getOrientation();
		rot.normalize();

		// Z�����]�ō��W�n�␳
		D3DXQUATERNION dq(rot.x(), rot.y(), rot.z(), rot.w());

		// ��]�s��ɕϊ�
		D3DXMATRIX matRot;
		D3DXMatrixRotationQuaternion(&matRot, &dq);

		// �s�� -> �I�C���[�p�iXYZ���j
		D3DXVECTOR3 euler;
		float sy = -matRot._32; // �E����W

		// Clamp�͈͂������ɂ��Ĉُ�p���
		sy = std::max(-1.0f, std::min(1.0f, sy));
		euler.x = asinf(sy);  // pitch (X)

		// cos(pitch) ��0�ɋ߂��ƃW���o�����b�N�Ȃ̂ŁA���̉������
		if (fabsf(cosf(euler.x)) > 1e-4f)
		{
			euler.y = atan2f(matRot._31, matRot._33);  // yaw (Y)
			euler.z = atan2f(matRot._12, matRot._22);  // roll (Z)
		}
		else
		{
			euler.y = 0.0f;
			euler.z = atan2f(-matRot._21, matRot._11); // ��֒l�iRoll�������o�j
		}

		// �A���␳
		static D3DXVECTOR3 prevEuler(0, 0, 0);
		auto FixAngleJump = [](float prev, float current) -> float

		{
			if (_isnan(current))
			{
				return prev;
			}

			float diff = current - prev;

			if (diff > D3DX_PI)
			{
				current -= 2 * D3DX_PI;
			}
			else if (diff < -D3DX_PI)
			{
				current += 2 * D3DX_PI;
			}

			return current;
		};

		euler.x = FixAngleJump(prevEuler.x, euler.x);
		euler.y = FixAngleJump(prevEuler.y, euler.y);
		euler.z = FixAngleJump(prevEuler.z, euler.z);

		prevEuler = euler;

		// �Z�b�g
		SetRot(euler);
	}
}
//=============================================================================
// �R���C�_�[�̍X�V����
//=============================================================================
void CBlock::UpdateCollider(void)
{
	if (!m_pRigidBody)
	{
		return;
	}

	// �ʒu�̎擾
	D3DXVECTOR3 Pos = GetPos();

	// �폜���čĐ���
	ReleasePhysics();
	CreatePhysics(Pos, m_colliderSize);
}
//=============================================================================
// �`�揈��
//=============================================================================
void CBlock::Draw(void)
{
	// �I�u�W�F�N�gX�̕`�揈��
	CObjectX::Draw();
}
//=============================================================================
// �����蔻��`�揈��
//=============================================================================
void CBlock::DrawCollider(void)
{
	if (m_pRigidBody)
	{
		// �R���C�_�[�̕`��
		m_pDebug3D->DrawBlockCollider(m_pRigidBody, D3DXCOLOR(0.0f, 1.0f, 0.3f, 1.0f));
	}
}
//=============================================================================
// �F�̎擾
//=============================================================================
D3DXCOLOR CBlock::GetCol(void) const
{
	if (m_bSelected)
	{// �Ԃ�����
		return D3DXCOLOR(1.0f, 0.0f, 0.0f, 0.6f);
	}
	else
	{// ���␳
		return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); 
	}
}
//=============================================================================
// �����蔻��̐�������
//=============================================================================
void CBlock::CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size)
{
	// Physics�̔j��
	ReleasePhysics();

	m_colliderSize = size;
	m_pShape = CreateCollisionShape(size);// �R���C�_�[�̐���

	btTransform transform;
	transform.setIdentity();
	btVector3 origin(pos.x + m_colliderOffset.x, pos.y + m_colliderOffset.y, pos.z + m_colliderOffset.z);
	transform.setOrigin(origin);

	// ��]�𔽉f
	D3DXVECTOR3 euler = GetRot();
	D3DXMATRIX matRot;
	D3DXMatrixRotationYawPitchRoll(&matRot, euler.y, euler.x, euler.z);

	D3DXQUATERNION dq;
	D3DXQuaternionRotationMatrix(&dq, &matRot);
	btQuaternion quat(dq.x, dq.y, dq.z, dq.w);
	transform.setRotation(quat);

	// �ҏW���͋����I�ɐÓI
	btScalar mass = m_isEditMode ? GetMass() : (!IsDynamicBlock() ? 0.0f : GetMass());
	btVector3 inertia(0, 0, 0);

	if (mass != 0.0f)
	{
		m_pShape->calculateLocalInertia(mass, inertia);
	}

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	// ���W�b�h�{�f�B�̐���
	m_pRigidBody = new btRigidBody(rbInfo);
	m_pRigidBody->setUserPointer(this);

	int flags = m_pRigidBody->getCollisionFlags();
	flags |= GetCollisionFlags();

	// �h���N���X���̓��ꏈ��
	OnPhysicsCreated();

	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);	// �X���[�v��Ԃɂ��Ȃ�
	m_pRigidBody->setCollisionFlags(flags);					// �R���W�����t���O
	m_pRigidBody->setLinearFactor(GetLinearFactor());		// ��������
	m_pRigidBody->setAngularFactor(GetAngularFactor());		// ��]�������
	m_pRigidBody->setFriction(GetFriction());				// ���C
	m_pRigidBody->setRollingFriction(GetRollingFriction());	// �]���薀�C
	m_pRigidBody->setDamping(0.1f, 0.5f);					// ����(��R)linearDamping, angularDamping

	// �������[���h�̎擾
	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != nullptr)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}
}
//=============================================================================
// �X�P�[���ɂ��R���C�_�[�̐�������
//=============================================================================
void CBlock::CreatePhysicsFromScale(const D3DXVECTOR3& scale)
{
	// ���f���̌��T�C�Y�̎擾
	D3DXVECTOR3 modelSize = GetModelSize();

	D3DXVECTOR3 newColliderSize =
	{
		modelSize.x * scale.x,
		modelSize.y * scale.y,
		modelSize.z * scale.z
	};

	CreatePhysics(GetPos(), newColliderSize); // �Đ���
}
//=============================================================================
// �����蔻��̎蓮�ݒ�p
//=============================================================================
void CBlock::SetColliderManual(const D3DXVECTOR3& newSize)
{
	m_colliderSize = newSize;

	ReleasePhysics();							// �ȑO�̍��̂��폜

	CreatePhysics(GetPos(), m_colliderSize);	// �Đ���
}
//=============================================================================
// Physics�̔j��
//=============================================================================
void CBlock::ReleasePhysics(void)
{
	auto world = CManager::GetPhysicsWorld();

	// ���̂�V�F�C�v�������O�ɔh���N���X�p�̓��ꏈ���j�����Ă�
	OnPhysicsReleased();

	// ���W�b�h�{�f�B�̔j��
	if (m_pRigidBody)
	{
		if (world)
		{
			world->removeRigidBody(m_pRigidBody);
		}

		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = nullptr;
	}

	// �V�F�C�v�̔j��
	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = nullptr;
	}
}
//=============================================================================
// �G�f�B�^�[�����ǂ����ŃL�l�}�e�B�b�N�ɂ��邩���肷�鏈��
//=============================================================================
void CBlock::SetEditMode(bool enable)
{
	m_isEditMode = enable;

	if (!m_pRigidBody)
	{
		return;
	}

	if (enable)
	{
		// �L�l�}�e�B�b�N�ɂ���
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);
	}
	else
	{
		// �L�l�}�e�B�b�N����
		m_pRigidBody->setCollisionFlags(m_pRigidBody->getCollisionFlags() & ~btCollisionObject::CF_KINEMATIC_OBJECT);
		m_pRigidBody->setActivationState(ACTIVE_TAG);
	}
}
//=============================================================================
// ���[���h�}�g���b�N�X�̎擾
//=============================================================================
D3DXMATRIX CBlock::GetWorldMatrix(void)
{
	D3DXMATRIX matScale, matRot, matTrans;

	// �X�P�[���s��
	D3DXVECTOR3 scale = GetSize(); // �g�嗦
	D3DXMatrixScaling(&matScale, scale.x, scale.y, scale.z);

	// ��]�s��
	D3DXVECTOR3 rot = GetRot(); // ���W�A���p
	D3DXMatrixRotationYawPitchRoll(&matRot, rot.y, rot.x, rot.z);

	// ���s�ړ��s��
	D3DXVECTOR3 pos = GetPos();
	D3DXMatrixTranslation(&matTrans, pos.x, pos.y, pos.z);

	// �����FS * R * T
	D3DXMATRIX world = matScale * matRot * matTrans;

	return world;
}
//=============================================================================
// ���X�|�[������
//=============================================================================
void CBlock::Respawn(D3DXVECTOR3 resPos)
{
	// ���������߂ɃL�l�}�e�B�b�N�ɂ���
	SetEditMode(true);

	// ���X�|�[���ʒu�ɐݒ�
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0, 0, 0));

	// �R���C�_�[�̍X�V
	UpdateCollider();

	// ���I�ɖ߂�
	SetEditMode(false);
}