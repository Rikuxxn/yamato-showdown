//=============================================================================
//
// �����L���O���� [ranking.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RANKING_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RANKING_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "scene.h"
#include "rankingmanager.h"
#include "ranktime.h"

//*****************************************************************************
// �����L���O�N���X
//*****************************************************************************
class CRanking : public CScene
{
public:
	CRanking();
	~CRanking();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

private:
	CRankTime* m_pRankTime;            // �N���A�^�C�������L���O�\���p
	CRankingManager* m_pRankingManager;// �����L���O�ւ̃|�C���^
};


#endif

