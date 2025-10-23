//=============================================================================
//
// ゲーム処理 [game.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "game.h"
#include "manager.h"
#include "result.h"
#include "particle.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CPlayer* CGame::m_pPlayer = nullptr;
CTime* CGame::m_pTime = nullptr;					// タイムへのポインタ
CColon* CGame::m_pColon = nullptr;					// コロンへのポインタ
CBlock* CGame::m_pBlock= nullptr;					// ブロックへのポインタ
CBlockManager* CGame::m_pBlockManager= nullptr;		// ブロックマネージャーへのポインタ
CObjectBillboard* CGame::m_pBillboard = nullptr;	// ビルボードへのポインタ
CPauseManager* CGame::m_pPauseManager = nullptr;	// ポーズマネージャーへのポインタ
bool CGame::m_isPaused = false;						// trueならポーズ中
int CGame::m_nSeed = 0;								// マップのシード値

//=============================================================================
// コンストラクタ
//=============================================================================
CGame::CGame() : CScene(CScene::MODE_GAME)
{
	// 値のクリア
}
//=============================================================================
// デストラクタ
//=============================================================================
CGame::~CGame()
{
	// なし
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CGame::Init(void)
{
#ifdef _DEBUG
	// グリッドの生成
	m_pGrid = CGrid::Create();

	// グリッドの初期化
	m_pGrid->Init();
#endif

	// ブロックマネージャーの生成
	m_pBlockManager = new CBlockManager;

	// ブロックマネージャーの初期化
	m_pBlockManager->Init();

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 100.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	m_nSeed = (int)time(nullptr);  // シード値をランダム設定

	// マップランダム生成
	m_pBlockManager->GenerateRandomMap(m_nSeed);

	float fTimePosX = 760.0f;
	float fTimeWidth = 42.0f;
	float fTimeHeight = 58.0f;

	// タイムの生成
	m_pTime = CTime::Create(0, 0, fTimePosX, 10.0f, fTimeWidth, fTimeHeight);

	// コロンの生成
	m_pColon = CColon::Create(D3DXVECTOR3(fTimePosX + 2 * fTimeWidth, 10.0f, 0.0f), fTimeWidth / 2, fTimeHeight);

	//// ポーズUIの生成
	//m_pUi = CUi::Create<CPauseUi>("data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 855.0f, 0.0f), 160.0f, 35.0f);

	// ポーズマネージャーの生成
	m_pPauseManager = new CPauseManager();

	// ポーズマネージャーの初期化
	m_pPauseManager->Init();

	CResult::SetGet(false);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CGame::Uninit(void)
{
	// 音の停止
	CManager::GetSound()->Stop(CSound::SOUND_LABEL_GAMEBGM);

	// ブロックマネージャーの破棄
	if (m_pBlockManager != nullptr)
	{
		m_pBlockManager->Uninit();

		delete m_pBlockManager;
		m_pBlockManager = nullptr;
	}

	// ポーズマネージャーの破棄
	if (m_pPauseManager != nullptr)
	{
		// ポーズマネージャーの終了処理
		m_pPauseManager->Uninit();

		delete m_pPauseManager;
		m_pPauseManager = nullptr;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CGame::Update(void)
{
	CFade* pFade = CManager::GetFade();
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();
	CInputJoypad* pJoypad = CManager::GetInputJoypad();

	// TABキーでポーズON/OFF
	if (pKeyboard->GetTrigger(DIK_TAB) || pJoypad->GetTrigger(CInputJoypad::JOYKEY_START))
	{
		// ポーズSE
		CManager::GetSound()->Play(CSound::SOUND_LABEL_PAUSE);

		// ポーズ切り替え前の状態を記録
		bool wasPaused = m_isPaused;

		m_isPaused = !m_isPaused;

		// ポーズ状態に応じて音を制御
		if (m_isPaused && !wasPaused)
		{
			// 一時停止する
			CManager::GetSound()->PauseAll();
		}
		else if (!m_isPaused && wasPaused)
		{
			// 再開する
			CManager::GetSound()->ResumeAll();
		}
	}

	// ブロックマネージャーの更新処理
	m_pBlockManager->Update();

	// 終了判定チェック
	if (m_pBlockManager)
	{
		for (auto block : m_pBlockManager->GetAllBlocks())
		{
			if (block->IsGet())
			{// 隠された秘宝を手に入れたか
				CResult::SetGet(true);
			}

			if (block->IsEnd())
			{
				// リザルトにセット
				CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

				// リザルト画面に移行
				pFade->SetFade(MODE_RESULT);

				break;
			}
		}
	}

#ifdef _DEBUG
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pFade->GetFade() == CFade::FADE_NONE && pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// リザルトにセット
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// リザルト画面に移行
		pFade->SetFade(MODE_RESULT);
	}
#endif
}
//=============================================================================
// 描画処理
//=============================================================================
void CGame::Draw(void)
{
#ifdef _DEBUG
	// グリッドの描画
	m_pGrid->Draw();
#endif
	// ポーズ中だったら
	if (m_isPaused)
	{
		// ポーズマネージャーの描画処理
		m_pPauseManager->Draw();
	}
}
//=============================================================================
// ポーズの設定
//=============================================================================
void CGame::SetEnablePause(bool bPause)
{
	m_isPaused = bPause;

	if (bPause)
	{
		// 音を一時停止
		CManager::GetSound()->PauseAll();
	}
	else
	{
		// 音を再開
		CManager::GetSound()->ResumeAll();
	}
}
