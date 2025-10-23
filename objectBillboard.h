//=============================================================================
//
// �r���{�[�h���� [objectBillboard.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECTBILLBOARD_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECTBILLBOARD_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"


//*****************************************************************************
// �r���{�[�h�N���X
//*****************************************************************************
class CObjectBillboard : public CObject
{
public:
	CObjectBillboard(int nPriority = 5);
	~CObjectBillboard();

	//*************************************************************************
	// �r���{�[�h�����e���v���[�g
	//*************************************************************************
	// billboardType : �����������r���{�[�h�N���X��
	template <typename billboardType>
	static CObjectBillboard* Create(const char* path, D3DXVECTOR3 pos, float fWidth, float fHeight)
	{
		// �n���ꂽ�^��CObjectBillboard���p�����Ă��Ȃ������ꍇ
		static_assert(std::is_base_of<CObjectBillboard, billboardType>::value, "�^��CObjectBillboard���p�����Ă��܂���B");

		billboardType* pBillboard = new billboardType();// �^�̃C���X�^���X����

		if (!pBillboard)
		{
			return nullptr;
		}

		pBillboard->SetPath(path);
		pBillboard->m_pos = pos;
		pBillboard->m_fSize = fWidth;// �G�t�F�N�g�̔��a
		pBillboard->SetHeight(fHeight);
		pBillboard->m_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// ����������
		pBillboard->Init();

		return pBillboard;
	}

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
	void SetCol(D3DXCOLOR col) { m_col = col; }
	void SetSize(float fRadius) { m_fSize = fRadius; }
	void SetHeight(float fHeight) { m_fHeight = fHeight; }
	void SetPath(const char* path) { strcpy_s(m_szPath, MAX_PATH, path); }

	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXCOLOR GetCol(void) { return m_col; }

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^
	D3DXVECTOR3 m_pos;						// �ʒu
	D3DXCOLOR m_col;						// �F
	D3DXMATRIX m_mtxWorld;					// ���[���h�}�g���b�N�X
	float m_fSize;							// �T�C�Y(�G�t�F�N�g���a)
	float m_fHeight;						// �T�C�Y(�r���{�[�h)
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X
	char m_szPath[MAX_PATH];				// �t�@�C���p�X
};

//*****************************************************************************
// �q���g�r���{�[�h�N���X
//*****************************************************************************
class CHintBillboard : public CObjectBillboard
{
public:
	CHintBillboard();
	~CHintBillboard();

	// �r���{�[�h�̏��
	typedef enum
	{
		STATE_FADEIN = 0,
		STATE_NORMAL,
		STATE_FADEOUT,
		STATE_MAX
	}STATE;

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	float m_fAlpha;	// �A���t�@�l
	STATE m_state;	// ���

};

#endif