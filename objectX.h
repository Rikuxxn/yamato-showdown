//=============================================================================
//
// X�t�@�C������ [objectX.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECTX_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECTX_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// X�t�@�C���N���X
//*****************************************************************************
class CObjectX : public CObject
{
public:
	CObjectX(int nPriority = 3);
	virtual ~CObjectX();

	static CObjectX* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 size);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }
	void SetSize(D3DXVECTOR3 size) { m_size = size; }
	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }

	const char* GetPath(void) { return m_szPath; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; }
	D3DXVECTOR3 GetSize(void) const { return m_size; }		// �g�嗦
	D3DXVECTOR3 GetModelSize(void) { return m_modelSize; }	// ���f���̌��T�C�Y
	virtual D3DXCOLOR GetCol(void) const { return D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f); }
	D3DXCOLOR GetMaterialColor(void) const;

private:
	int* m_nIdxTexture;
	D3DXVECTOR3 m_pos;					// �ʒu
	D3DXVECTOR3 m_rot;					// ����
	D3DXVECTOR3 m_move;					// �ړ���
	D3DXVECTOR3 m_size;					// �T�C�Y
	LPD3DXMESH m_pMesh;					// ���b�V���ւ̃|�C���^
	LPD3DXBUFFER m_pBuffMat;			// �}�e���A���ւ̃|�C���^
	DWORD m_dwNumMat;					// �}�e���A����
	D3DXMATRIX m_mtxWorld;				// ���[���h�}�g���b�N�X
	char m_szPath[MAX_PATH];			// �t�@�C���p�X
	D3DXVECTOR3 m_modelSize;			// ���f���̌��T�C�Y�i�S�̂̕��E�����E���s���j
};

#endif