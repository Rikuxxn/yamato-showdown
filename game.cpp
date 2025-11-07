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
#include "charactermanager.h"
#include "player.h"
#include "enemy.h"

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
CPlayer* CGame::m_pPlayer = nullptr;				// プレイヤーへのポインタ
CEnemy* CGame::m_pEnemy = nullptr;					// 敵へのポインタ
CTime* CGame::m_pTime = nullptr;					// タイムへのポインタ
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
	m_pRankingManager = nullptr;
	m_pLight = nullptr;
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

	// ライトの生成
	m_pLight = new CLight;

	// ライトの初期化
	m_pLight->Init();

	CCharacterManager characterManager;

	// プレイヤーの生成
	m_pPlayer = CPlayer::Create(D3DXVECTOR3(0.0f, 10.0f, -300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	//m_pPlayer = CPlayer::Create(D3DXVECTOR3(-660.0f, 100.0f, -3898.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	characterManager.AddCharacter(m_pPlayer);

	// 敵の生成
	m_pEnemy = CEnemy::Create(D3DXVECTOR3(0.0f, 10.0f, 300.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	characterManager.AddCharacter(m_pEnemy);

	m_nSeed = (int)time(nullptr);  // シード値をランダム設定

	// マップランダム生成
	m_pBlockManager->GenerateRandomMap(m_nSeed);

	// タイムの生成
	m_pTime = CTime::Create(3, 0, 760.0f, 10.0f, 42.0f, 58.0f);

	//// ポーズUIの生成
	//m_pUi = CUi::Create<CPauseUi>("data/TEXTURE/ui_pause.png",D3DXVECTOR3(210.0f, 855.0f, 0.0f), 160.0f, 35.0f);

	// ポーズマネージャーの生成
	m_pPauseManager = new CPauseManager();

	// ポーズマネージャーの初期化
	m_pPauseManager->Init();

	// ランキングマネージャーのインスタンス生成
	m_pRankingManager = make_unique<CRankingManager>();

	// フラグのリセット
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

	// ライトの破棄
	if (m_pLight != nullptr)
	{
		delete m_pLight;
		m_pLight = nullptr;
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

	UpdateLight();

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
			{// 埋蔵金を手に入れたか
				CResult::SetGet(true);
			}
		}
	}

	// 敵を倒したらクリア
	if (pFade->GetFade() == CFade::FADE_NONE && CGame::GetEnemy()->IsDeath())
	{
		// ランキングに登録
		m_pRankingManager->AddRecordWithLimit(3, 0, m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// リザルトにセット
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// リザルト画面に移行
		pFade->SetFade(MODE_RESULT);
	}
	else if (pFade->GetFade() == CFade::FADE_NONE && m_pTime->IsTimeUp() ||
		m_pPlayer->IsDead())
	{// 時間切れ または プレイヤー死亡

		// リザルト画面に移行
		pFade->SetFade(MODE_RESULT);
	}

#ifdef _DEBUG
	CInputKeyboard* pInputKeyboard = CManager::GetInputKeyboard();

	if (pFade->GetFade() == CFade::FADE_NONE && pInputKeyboard->GetTrigger(DIK_RETURN))
	{
		// ランキングに登録
		m_pRankingManager->AddRecordWithLimit(3, 0, m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// リザルトにセット
		CResult::SetClearTime(m_pTime->GetMinutes(), m_pTime->GetnSeconds());

		// リザルト画面に移行
		pFade->SetFade(MODE_RESULT);
	}
#endif
}
//=============================================================================
// ライトの色更新処理
//=============================================================================
void CGame::UpdateLight(void)
{
	float progress = m_pTime->GetProgress(); // 0.0～0.1

	// ======== 各時間帯のメインライト色 ========
	D3DXCOLOR evening(1.0f, 0.65f, 0.35f, 1.0f); // 夕日：濃いオレンジ
	D3DXCOLOR night(0.15f, 0.18f, 0.35f, 1.0f);  // 夜：青みが強く暗い（でも完全には黒くしない）
	D3DXCOLOR morning(0.95f, 0.8f, 0.7f, 1.0f);  // 明け方：柔らかいピンクベージュ

	D3DXCOLOR mainColor;

	// ======== 時間帯ごとに補間 ========
	if (progress < 0.33f)
	{// 夕方
		float t = progress / 0.33f;
		D3DXColorLerp(&mainColor, &evening, &night, t);
	}
	else if (progress < 0.66f)
	{// 夜
		float t = (progress - 0.33f) / 0.33f;
		D3DXColorLerp(&mainColor, &night, &morning, t);
	}
	else
	{// 明け方
		float t = (progress - 0.66f) / 0.34f;
		D3DXColorLerp(&mainColor, &morning, &evening, t); // 少し戻すとループっぽく自然
	}

	// ======== 光の向き補間 ========
	D3DXVECTOR3 dirEvening(0.5f, -1.0f, 0.3f);
	D3DXVECTOR3 dirNight(0.0f, -1.0f, 0.0f);
	D3DXVECTOR3 dirMorning(-0.3f, -1.0f, -0.2f);
	D3DXVECTOR3 mainDir;

	if (progress < 0.5f)
	{
		float t = progress / 0.5f;
		D3DXVec3Lerp(&mainDir, &dirEvening, &dirNight, t);
	}
	else
	{
		float t = (progress - 0.5f) / 0.5f;
		D3DXVec3Lerp(&mainDir, &dirNight, &dirMorning, t);
	}
	D3DXVec3Normalize(&mainDir, &mainDir);

	// 再設定
	CLight::Uninit();

	m_pBlockManager->UpdateLight();

	// メインライト（太陽・月相当）
	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		mainColor,
		mainDir,
		D3DXVECTOR3(0.0f, 300.0f, 0.0f)
	);

	// サブライト（空の反射光）も時間帯で変化
	D3DXCOLOR skyEvening(0.4f, 0.45f, 0.8f, 1.0f);
	D3DXCOLOR skyNight(0.1f, 0.15f, 0.3f, 1.0f);
	D3DXCOLOR skyMorning(0.6f, 0.7f, 1.0f, 1.0f);
	D3DXCOLOR skyColor;

	if (progress < 0.5f)
	{
		D3DXColorLerp(&skyColor, &skyEvening, &skyNight, progress / 0.5f);
	}
	else
	{
		D3DXColorLerp(&skyColor, &skyNight, &skyMorning, (progress - 0.5f) / 0.5f);
	}

	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		skyColor,
		D3DXVECTOR3(0.0f, -1.0f, 0.0f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f)
	);

	// 補助光：ほんのり赤み（朝夕）を残す
	float warmFactor = 1.0f - fabs(progress - 0.5f) * 2.0f; // 夕・朝で強め
	warmFactor = max(0.0f, warmFactor);

	CLight::AddLight(
		D3DLIGHT_DIRECTIONAL,
		D3DXCOLOR(0.5f + 0.2f * warmFactor, 0.3f, 0.25f, 1.0f),
		D3DXVECTOR3(-0.3f, 0.0f, -0.7f),
		D3DXVECTOR3(0.0f, 0.0f, 0.0f)
	);
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
// デバイスリセット通知
//=============================================================================
void CGame::OnDeviceReset(void)
{
	// ゲームライトの更新処理
	UpdateLight();
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
