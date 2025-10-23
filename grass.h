//=============================================================================
//
// ������ [grass.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GRASS_H_
#define _GRASS_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object3D.h"
#include "block.h"

//*****************************************************************************
// ���N���X
//*****************************************************************************
class CGrassBlock : public CBlock
{
public:
	CGrassBlock();
	~CGrassBlock();

	void Update(void);
	void Draw(void);

	int GetCollisionFlags(void) const override { return btCollisionObject::CF_NO_CONTACT_RESPONSE; }

private:
	D3DXVECTOR3 m_rotVel; // �p���x
};

#endif
