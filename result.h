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
#include "resulttime.h"
#include "light.h"
#include "blockmanager.h"

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

	static int GetClearMinutes(void) { return m_nClearMinutes; }
	static int GetClearSeconds(void) { return m_nClearSeconds; }

	static void ResetLight(void);
	void OnDeviceReset(void) override;
	void ReleaseThumbnail(void) override;
	void ResetThumbnail(void) override;

private:
	static CResultTime* m_pTime;			// タイムへのポインタ
	static int m_nClearMinutes;				// クリアタイム(分)
	static int m_nClearSeconds;				// クリアタイム(秒)
	CLight* m_pLight;						// ライトへのポインタ
	CBlockManager* m_pBlockManager;	// ブロックマネージャーへのポインタ
};

#endif
