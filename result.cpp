//=============================================================================
//
// リザルト処理 [result.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "result.h"
#include "input.h"
#include "manager.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CTime* CResult::m_pTime = nullptr;					// タイムへのポインタ
int CResult::m_nClearMinutes = 0;
int CResult::m_nClearSeconds = 0;
bool CResult::m_isTreasureGet = false;

//=============================================================================
// コンストラクタ
//=============================================================================
CResult::CResult() : CScene(CScene::MODE_RESULT)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CResult::~CResult()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CResult::Init(void)
{
	// マウスカーソルを表示する
	CManager::GetInputMouse()->SetCursorVisibility(true);

	//// 背景の生成
	//CBackground::Create(D3DXVECTOR3(960.0f, 540.0f, 0.0f), 960.0f, 540.0f, "data/TEXTURE/resultBG.png");

	// 合計秒で扱う
	int totalLimit = 3 * 60 + 0;
	int totalRemain = m_nClearMinutes * 60 + m_nClearSeconds;

	// クリアタイム（経過時間）を計算
	int totalClear = totalLimit - totalRemain;
	if (totalClear < 0)
	{
		totalClear = 0; // 念のため
	}

	// 分と秒に戻す
	int clearMinutes = totalClear / 60;
	int clearSeconds = totalClear % 60;

	// タイムの生成
	m_pTime = CTime::Create(clearMinutes, clearSeconds, 1100.0f, 695.0f, 72.0f, 88.0f);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CResult::Uninit(void)
{
	SetGet(false);
}
//=============================================================================
// 更新処理
//=============================================================================
void CResult::Update(void)
{
	CInputMouse* pInputMouse = CManager::GetInputMouse();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();
	CFade* pFade = CManager::GetFade();

	if (pFade->GetFade() == CFade::FADE_NONE && (pInputMouse->GetTrigger(0) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_A)))
	{
		// ランキング画面に移行
		pFade->SetFade(MODE_RANKING);
	}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN))
	{
		// ランキング画面に移行
		pFade->SetFade(MODE_RANKING);
	}
#endif
}
//=============================================================================
// 描画処理
//=============================================================================
void CResult::Draw(void)
{


}
