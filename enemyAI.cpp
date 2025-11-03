//=============================================================================
//
// 敵の学習AI処理 [enemyAI.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "enemyAI.h"
#include "player.h"
#include "enemy.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CEnemyAI::CEnemyAI()
{
	// 値のクリア
	m_timer = 0.0f;
}
//=============================================================================
// デストラクタ
//=============================================================================
CEnemyAI::~CEnemyAI()
{
	// なし
}
//=============================================================================
// 更新処理
//=============================================================================
void CEnemyAI::Update(CEnemy* pEnemy, CPlayer* pPlayer)
{
    if (!pEnemy || !pPlayer)
    {
        return;
    }

    m_timer++;  // 経過時間

    // プレイヤーとの距離を算出
    D3DXVECTOR3 diff = pPlayer->GetPos() - pEnemy->GetPos();
    float distance = D3DXVec3Length(&diff);

    // 行動決定タイミング（2秒ごと）
    if (m_timer < 120.0f)
    {
        return;
    }

    m_timer = 0.0f;

    if (pEnemy->GetMotion()->IsAttacking(CEnemy::ATTACK_01))
    {
        // 攻撃中は新しいリクエストを出さない
        pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_NEUTRAL);
        return;
    }

    // 攻撃・移動・待機を距離とランダムで決定
    if (distance < 150.0f)
    {
        // 近距離 → 攻撃
        pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_ATTACK_02);
    }
    else if (distance < 300.0f)
    {
        // 中距離 → 溜め行動（攻撃準備）
        if ((rand() % 100) < 70)
        {
            pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_ACCUMULATE);
        }
        else
        {
            pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_NEUTRAL);
        }
    }
    else
    {
        // 遠距離 → プレイヤーに向かって移動
        pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_MOVE);
    }
}
