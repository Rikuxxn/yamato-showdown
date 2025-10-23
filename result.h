//=============================================================================
//
// ���U���g���� [result.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RESULT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RESULT_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "scene.h"
#include "time.h"

//*****************************************************************************
// ���U���g�N���X
//*****************************************************************************
class CResult : public CScene
{
public:
	CResult();
	~CResult();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	static void SetClearTime(int min, int sec) { m_nClearMinutes = min; m_nClearSeconds = sec; }
	static void SetGet(bool flag) { m_isTreasureGet = flag; }

	static bool GetTreasure(void) { return m_isTreasureGet; }
	static int GetClearMinutes(void) { return m_nClearMinutes; }
	static int GetClearSeconds(void) { return m_nClearSeconds; }

private:
	static CTime* m_pTime;					// �^�C���ւ̃|�C���^
	static CColon* m_pColon;				// �R�����ւ̃|�C���^
	static int m_nClearMinutes;				// �N���A�^�C��(��)
	static int m_nClearSeconds;				// �N���A�^�C��(�b)
	static bool m_isTreasureGet;			// ������肵����
};

#endif
