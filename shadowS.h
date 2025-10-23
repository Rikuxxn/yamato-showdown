//=============================================================================
//
// �X�e���V���V���h�E���� [shadowS.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _SHADOWS_H_// ���̃}�N����`������Ă��Ȃ�������
#define _SHADOWS_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectX.h"

//*****************************************************************************
// �X�e���V���V���h�E�N���X
//*****************************************************************************
class CShadowS : public CObjectX
{
public:
	CShadowS(int nPriority = 4);
	~CShadowS();

	static CShadowS* Create(const char* pFilepath,D3DXVECTOR3 pos);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetPosition(D3DXVECTOR3 pos) { m_pos = pos; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// 2D�|���S���`��p�ʒu
	D3DCOLOR m_col;							// 2D�|���S���`��p�F
	float m_fWidth, m_fHeight;				// 2D�|���S���`��p�T�C�Y

};
#endif
