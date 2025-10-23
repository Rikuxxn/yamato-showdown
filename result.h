//=============================================================================
//
// リザルト処理 [result.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RESULT_H_// このマクロ定義がされていなかったら
#define _RESULT_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "scene.h"
#include "time.h"

//*****************************************************************************
// リザルトクラス
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
	static CTime* m_pTime;					// タイムへのポインタ
	static CColon* m_pColon;				// コロンへのポインタ
	static int m_nClearMinutes;				// クリアタイム(分)
	static int m_nClearSeconds;				// クリアタイム(秒)
	static bool m_isTreasureGet;			// 秘宝を入手したか
};

#endif
