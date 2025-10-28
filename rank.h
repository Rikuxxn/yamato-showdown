//=============================================================================
//
// �����L���O���ʏ��� [rank.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RANK_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RANK_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"

//*****************************************************************************
// �����L���O���ʃN���X
//*****************************************************************************
class CRank : public CObject
{
public:
	// ����
	enum RANK
	{
		RANK_FIRST = 0,	// 1��
		RANK_SECOND,	// 2��
		RANK_THIRD,		// 3��
		RANK_OTHER,		// ���̑�
		RANK_MAX
	};

	CRank(int nPriority = 7);
	~CRank();

	static CRank* Create(D3DXVECTOR3 pos, float fWidth, float fHeight, float fRank);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@
	D3DXVECTOR3 m_pos;						// �ʒu
	float m_fWidth, m_fHeight;				// �T�C�Y�i���E�����j
	float m_fRank;							// ����
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X
};

#endif