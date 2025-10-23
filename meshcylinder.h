//=============================================================================
//
// ���b�V���V�����_�[���� [meshcylinder.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MESHCYLINDER_H_
#define _MESHCYLINDER_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// ���b�V���V�����_�[�N���X
//*****************************************************************************
class CMeshCylinder : public CObject
{
public:
	CMeshCylinder(int nPriority = 5);
	~CMeshCylinder();

	static CMeshCylinder* Create(D3DXVECTOR3 pos, D3DXCOLOR col, float fRad, float fWeight, float incRad, int nLife, float decAlpha);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXCOLOR GetCol(void) { return m_col; }

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetRadius(float nRad) { m_fRadius = nRad; }
	void SetIncRadius(float incRad) { m_incRadius = incRad; }
	void SetLife(int nLife) { m_nLife = nLife; m_nMaxLife = nLife; }
	void SetDecAlpha(float alpha) { m_decAlpha = alpha; }
	void SetLineweight(float val) { m_fLineweight = val; }
private:
	static constexpr float MESHCYLINDER_HEIGHT = 20.0f;	// �c��
	static constexpr int MESHCYLINDER_X = 20;			// X�����̃u���b�N��
	static constexpr int MESHCYLINDER_Z = 1;			// Z�����̃u���b�N��
	static constexpr int MESHCYLINDER_V = 1;			// ���������̕�����
	static constexpr int MESHCYLINDER_H = 20;			// ���������̕�����
	static constexpr int MESHCYLINDER_PRIMITIVE = (MESHCYLINDER_X * MESHCYLINDER_Z) * 2 + (4 * (MESHCYLINDER_Z - 1));// �v���~�e�B�u��
	static constexpr int MESHCYLINDER_VERTEX = (MESHCYLINDER_X + 1) * (MESHCYLINDER_Z + 1);// ���_��
	static constexpr int MESHCYLINDER_INDEX = MESHCYLINDER_PRIMITIVE + 2;// �C���f�b�N�X��

	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;	// ���_�o�b�t�@�ւ̃|�C���^
	LPDIRECT3DINDEXBUFFER9 m_pIdxBuff;	// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X
	D3DXVECTOR3 m_pos;					// �ʒu
	D3DXVECTOR3 m_rot;					// ����
	D3DXCOLOR m_col;					// �F
	int m_nLife;						// ���݂̎���
	int m_nMaxLife;						// �ݒ莞�̎���
	float m_fRadius;					// ���a
	float m_fLineweight;				// ����
	float m_incRadius;					// ���a�̑�����
	float m_decAlpha;					// �A���t�@�l�̌�����
};

#endif
