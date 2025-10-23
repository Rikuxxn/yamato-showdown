//=============================================================================
//
// �^�C������ [time.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _TIME_H_// ���̃}�N����`������Ă��Ȃ�������
#define _TIME_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"
#include "number.h"


//*****************************************************************************
// �R�����N���X
//*****************************************************************************
class CColon : public CObject
{
public:
	CColon(int nPriority = 7);
	~CColon();

	static CColon* Create(D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);

private:
	LPDIRECT3DVERTEXBUFFER9 m_pVtxBuff;		// ���_�o�b�t�@
	D3DXVECTOR3 m_pos;						// �ʒu
	float m_fWidth, m_fHeight;				// �T�C�Y�i���E�����j
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X

};

//*****************************************************************************
// �^�C���N���X
//*****************************************************************************
class CTime : public CObject
{
public:
	CTime(int nPriority = 7);
	~CTime();

	static CTime* Create(int minutes, int seconds, float baseX, float baseY, float digitWidth, float digitHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Count(void);
	void Draw(void);
	D3DXVECTOR3 GetPos(void);
	int GetMinutes(void) { return m_nMinutes; }
	int GetnSeconds(void) { return m_nSeconds; }

private:
	static const int DIGITS = 4;
	CNumber* m_apNumber[DIGITS];
	int m_nMinutes;							// ��
	int m_nSeconds;							// �b
	int m_nFrameCount;						// �t���[���J�E���g
	float m_digitWidth;						// ����1��������̕�
	float m_digitHeight;					// ����1��������̍���
	D3DXVECTOR3 m_basePos;					// �\���̊J�n�ʒu
	CColon* m_pColon;						// �R�����ւ̃|�C���^
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X

};

#endif