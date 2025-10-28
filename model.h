//=============================================================================
//
// ���f������ [model.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MODEL_H_// ���̃}�N����`������Ă��Ȃ�������
#define _MODEL_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// ���f���N���X
//*****************************************************************************
class CModel
{
public:
	CModel();
	~CModel();

	static CModel* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void DrawNormal(LPDIRECT3DDEVICE9 pDevice);

	//*****************************************************************************
	// setter�֐�
	//*****************************************************************************
	void SetOutlineShaderConstants(LPDIRECT3DDEVICE9 pDevice);
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = D3DXToRadian(rot); }
	void SetParent(CModel* pModel) { m_pParent = pModel; }
	void SetOffsetPos(const D3DXVECTOR3& pos) { m_OffsetPos = pos; }
	void SetOffsetRot(const D3DXVECTOR3& rot) { m_OffsetRot = rot; }

	//*****************************************************************************
	// getter�֐�
	//*****************************************************************************
	D3DXMATRIX GetMtxWorld(void) { return m_mtxWorld; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DXVECTOR3 GetOffsetPos(void) const { return m_OffsetPos; }
	D3DXVECTOR3 GetOffsetRot(void) const { return m_OffsetRot; }
	const char* GetPath(void) { return m_Path; }
	CModel* GetParent(void) { return m_pParent; }

private:
	int* m_nIdxTexture;
	D3DXVECTOR3 m_pos;					// �ʒu
	D3DXVECTOR3 m_rot;					// ����
	D3DXVECTOR3 m_move;					// �ړ���
	LPD3DXMESH m_pMesh;					// ���b�V���ւ̃|�C���^
	LPD3DXBUFFER m_pBuffMat;			// �}�e���A���ւ̃|�C���^
	DWORD m_dwNumMat;					// �}�e���A����
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X
	CModel* m_pParent;					// �e���f���ւ̃|�C���^
	char m_Path[MAX_PATH];				// �t�@�C���p�X
	D3DXVECTOR3 m_OffsetPos;			// �I�t�Z�b�g
	D3DXVECTOR3 m_OffsetRot;			// �I�t�Z�b�g

	// �V�F�[�_�p�����o�ϐ�
	LPDIRECT3DVERTEXSHADER9 m_pOutlineVS;
	LPDIRECT3DPIXELSHADER9  m_pOutlinePS;
	LPD3DXCONSTANTTABLE     m_pVSConsts;
	LPD3DXCONSTANTTABLE     m_pPSConsts;
};

#endif
