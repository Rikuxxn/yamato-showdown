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
CColon* CResult::m_pColon = nullptr;				// コロンへのポインタ
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

	float fTimePosX = 1100.0f;
	float fTimeWidth = 72.0f;
	float fTimeHeight = 88.0f;

	// タイムの生成
	m_pTime = CTime::Create(m_nClearMinutes, m_nClearSeconds, fTimePosX, 695.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 695.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

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
		// ゲーム画面に移行
		pFade->SetFade(MODE_TITLE);
	}

#ifdef _DEBUG
	if (pFade->GetFade() == CFade::FADE_NONE && CManager::GetInputKeyboard()->GetTrigger(DIK_RETURN))
	{
		// ゲーム画面に移行
		pFade->SetFade(MODE_TITLE);
	}
#endif
}
//=============================================================================
// 描画処理
//=============================================================================
void CResult::Draw(void)
{


}
