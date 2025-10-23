//=============================================================================
//
// �u���b�N���� [block.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectX.h"
#include "debugproc3D.h"
#include <unordered_map>
#include <functional>

// �O���錾
class CBlock;

using BlockCreateFunc = std::function<CBlock* ()>;

struct ColliderPart
{
	D3DXVECTOR3 size;
	D3DXVECTOR3 offset;
};

//*****************************************************************************
// �u���b�N�N���X
//*****************************************************************************
class CBlock : public CObjectX
{
public:
	CBlock(int nPriority = 3);
	virtual ~CBlock() = default;

	//*****************************************************************************
	// �u���b�N�̎��
	//*****************************************************************************
	typedef enum
	{
		TYPE_WALL_01 = 0,
		TYPE_FLOOR,
		TYPE_TORCH_01,
		TYPE_ROCK,
		TYPE_GRASS,
		TYPE_WATER,
		TYPE_FLOOR2,

		TYPE_WOODBOX,
		TYPE_SEESAW,
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// �u���b�N�̐���
	virtual btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size)
	{ 
		return new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));	// �f�t�H���g�̓{�b�N�X �h���N���X�œ����̂������Shape��ݒ�
	}
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);				// �R���C�_�[�̐���
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);								// �u���b�N�X�P�[���ɂ��R���C�_�[�̐���
	virtual void OnPhysicsCreated(void) {}												// �u���b�N�̓��ꏈ��
	virtual void OnPhysicsReleased(void) {}												// �u���b�N�̓��ꏈ���̔j��
	static void InitFactory(void);
	HRESULT Init(void);
	void Kill(void) { m_isDead = true; }												// �u���b�N�폜
	void Uninit(void);
	void Update(void);
	void UpdateCollider(void);
	void Draw(void);
	void DrawCollider(void);
	void ReleasePhysics(void);															// Physics�j���p
	virtual void Respawn(D3DXVECTOR3 resPos);

	//*****************************************************************************
	// flagment�֐�
	//*****************************************************************************
	bool IsSelected(void) const { return m_bSelected; }									// �u���b�N���I�𒆂̃t���O��Ԃ�
	virtual bool IsDynamicBlock(void) const { return false; }							// ���I�u���b�N�̔���
	virtual bool IsEnd(void) { return false; }
	virtual bool IsGet(void) { return false; }
	virtual bool IsSwitchOn(void) { return false; }

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetType(TYPE type) { m_Type = type; }											// �^�C�v�̐ݒ�
	void SetSelected(bool flag) { m_bSelected = flag; }									// �I�𒆂̃t���O��Ԃ�
	void SetColliderSize(const D3DXVECTOR3& size) { m_colliderSize = size; }			// �R���C�_�[�T�C�Y�̐ݒ�
	void SetColliderManual(const D3DXVECTOR3& newSize);									// �R���C�_�[�T�C�Y�̎蓮�ݒ�p
	void SetColliderOffset(const D3DXVECTOR3& offset) { m_colliderOffset = offset; }	// �R���C�_�[�̃I�t�Z�b�g�̐ݒ�
	void SetEditMode(bool enable);

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// �J���[�̎擾
	TYPE GetType(void) const { return m_Type; }											// �^�C�v�̎擾
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾
	D3DXVECTOR3 GetColliderSize(void) const { return m_colliderSize; }					// �R���C�_�[�T�C�Y�̎擾
	D3DXVECTOR3 GetColliderOffset(void) const { return m_colliderOffset; }				// �R���C�_�[�̃I�t�Z�b�g�̎擾
	bool IsEditMode(void) const { return m_isEditMode; }								// �G�f�B�b�g�����ǂ���
	virtual btScalar GetMass(void) const { return 2.0f; }								// ���ʂ̎擾
	bool IsDead(void) const { return m_isDead; }										// �폜�\��̎擾
	D3DXMATRIX GetWorldMatrix(void);

	virtual int GetCollisionFlags(void) const { return 0; }// �f�t�H���g�̓t���O�Ȃ�
	virtual btVector3 GetLinearFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btScalar GetRollingFriction(void) const { return 0.7f; }
	virtual btScalar GetFriction(void) const { return 1.0f; }
	virtual D3DXVECTOR3 GetVelocity(void) { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual float CarryTargetDis(void) { return 60.0f; }

private:
	TYPE m_Type;					// ���
	D3DXCOLOR m_col;				// �A���t�@�l
	D3DXCOLOR m_baseCol;			// �x�[�X�̃A���t�@�l
	bool m_bSelected;				// �I���t���O
	btRigidBody* m_pRigidBody;		// ���̂ւ̃|�C���^
	btCollisionShape* m_pShape;		// �����蔻��̌`�ւ̃|�C���^
	CDebugProc3D* m_pDebug3D;		// 3D�f�o�b�O�\���ւ̃|�C���^
	D3DXVECTOR3 m_prevSize;			// �O��̃T�C�Y
	D3DXVECTOR3 m_colliderSize;		// �R���C�_�[�T�C�Y
	D3DXVECTOR3 m_colliderOffset;	// �R���C�_�[�̈ʒu
	bool m_isEditMode;				// �ҏW�����ǂ���
	static std::unordered_map<TYPE, BlockCreateFunc> m_BlockFactoryMap;
	bool m_isDead;					// �폜�\��t���O
};

#endif