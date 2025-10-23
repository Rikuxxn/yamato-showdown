//=============================================================================
//
// 3D�I�u�W�F�N�g���� [object3D.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECT3D_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECT3D_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// 3D�I�u�W�F�N�g�N���X
//*****************************************************************************
class CObject3D : public CObject
{
public:
	CObject3D(int nPriority = 5);
	~CObject3D();

	static CObject3D* Create(void);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
	void SetWidth(float fWidth) { m_fWidth = fWidth; }
	void SetHeight(float fHeight) { m_fHeight = fHeight; }
	void SetCol(D3DXCOLOR col) { m_col = col; }

	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DXVECTOR3 m_rot;						// ����
	D3DXCOLOR m_col;						// �F
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	float m_fWidth;							// ��
	float m_fHeight;						// ����
};

#endif
