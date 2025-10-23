//=============================================================================
//
// �u���b�N���X�g���� [blocklist.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "block.h"

//*****************************************************************************
// �ؔ��u���b�N�N���X
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	CWoodBoxBlock();
	~CWoodBoxBlock();

	HRESULT Init(void);
	void Update(void);
	void Respawn(D3DXVECTOR3 resPos) override;
	void Set(D3DXVECTOR3 pos);

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 3.0f; }  // ���ʂ̎擾

private:
	static constexpr float RESPAWN_HEIGHT = -810.0f;
	D3DXVECTOR3 m_ResPos;	// ���X�|�[���ʒu
};

//*****************************************************************************
// �V�[�\�[�u���b�N�N���X
//*****************************************************************************
class CSeesawBlock : public CBlock
{
public:
	CSeesawBlock();
	~CSeesawBlock();

	HRESULT Init(void);
	void Uninit(void);
	void RemoveHinge(void);
	void Update(void);
	void OnPhysicsCreated(void) override
	{
		// �q���W�̐ݒ菈��
		SetHinge();
	}
	void OnPhysicsReleased(void) override
	{
		// �q���W�̔j��
		RemoveHinge();
	}

	void SetHinge(void);
	btHingeConstraint* GetHinge(void) { return m_pHinge; }

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetLinearFactor(void) const { return btVector3(0.0f, 0.0f, 0.0f); }
	btScalar GetFriction(void) const { return 5.7f; }
	btScalar GetRollingFriction(void) const { return 5.7f; }
	btScalar GetMass(void) const { return 10.0f; }// ���ʂ̎擾

private:
	btHingeConstraint* m_pHinge;
};

//*****************************************************************************
// ���ău���b�N�N���X
//*****************************************************************************
class CTorchBlock : public CBlock
{
public:
	CTorchBlock();
	~CTorchBlock();

	void Update(void);

private:

};

//*****************************************************************************
// ���u���b�N�N���X
//*****************************************************************************
class CWaterBlock : public CBlock
{
public:
	CWaterBlock(int nPriority = 4);
	~CWaterBlock();

	void Update(void);

private:
	int m_counter;	// �����J�E���^�[
};

#endif
