//=============================================================================
//
// �����L���O�^�C������ [ranktime.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RANKTIME_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RANKTIME_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"
#include "number.h"
#include "rankingmanager.h"
#include "time.h"

// �O���錾
class CRankColon;

//*****************************************************************************
// �����L���O�^�C���N���X
//*****************************************************************************
class CRankTime : public CObject
{
public:
	CRankTime(int nPriority = 7);
	~CRankTime();

	static CRankTime* Create(float baseX, float baseY, float digitWidth, float digitHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetRankList(const std::vector<std::pair<int, int>>& rankList);

	D3DXVECTOR3 GetPos(void);

private:
	static constexpr int MaxRanking = 5;	// �\����
	static constexpr int DIGITS = 4;
	CNumber* m_apNumber[MaxRanking][DIGITS];// �i���o�[�ւ̃|�C���^
	int m_nMinutes;							// ��
	int m_nSeconds;							// �b
	int m_nFrameCount;						// �t���[���J�E���g
	float m_digitWidth;						// ����1��������̕�
	float m_digitHeight;					// ����1��������̍���
	D3DXVECTOR3 m_basePos;					// �\���̊J�n�ʒu
	CRankColon* m_apRankClon[MaxRanking];	// �R�����ւ̃|�C���^
	int m_nIdxTexture;						// �e�N�X�`���C���f�b�N�X

};

//*****************************************************************************
// �����L���O�R�����N���X
//*****************************************************************************
class CRankColon : public CObject
{
public:
	CRankColon(int nPriority = 7);
	~CRankColon();

	static CRankColon* Create(D3DXVECTOR3 pos, float fWidth, float fHeight);
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

#endif