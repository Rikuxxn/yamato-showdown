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

	static TYPE GetStaticType(void) { return TYPE_WOODBOX; }

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
// ���ău���b�N�N���X
//*****************************************************************************
class CTorchBlock : public CBlock
{
public:
	CTorchBlock();
	~CTorchBlock();

	static TYPE GetStaticType(void) { return TYPE_TORCH_01; }

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

	static TYPE GetStaticType(void) { return TYPE_WATER; }

	void Update(void);

private:
	int m_counter;	// �����J�E���^�[
};

#endif
