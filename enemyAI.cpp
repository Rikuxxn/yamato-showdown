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
	m_timer = 0;
    m_cautionCooldown = 0;
    m_afterAttackCooldown = 0;
    m_prevAttackState = false;
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

    // 行動を記録
    RecordPlayerAction();

    bool isEnemyAttacking = pEnemy->GetMotion()->IsCurrentMotion(CEnemy::CLOSE_ATTACK_01);
    bool playerIsAttacking = pPlayer->GetMotion()->IsCurrentMotion(CPlayer::ATTACK_01);

    // 敵が攻撃中にプレイヤーが攻撃してきた場合
    if (isEnemyAttacking && playerIsAttacking)
    {
        // 攻撃継続はせず回避行動へ
        pEnemy->SetRequestedAction(CEnemy::AI_EVADE);

        // 攻撃連続を防ぐためクールタイム少し増加
        m_afterAttackCooldown = 20;

        return;
    }

    // プレイヤーとの距離を算出
    D3DXVECTOR3 diff = pPlayer->GetPos() - pEnemy->GetPos();
    float distance = D3DXVec3Length(&diff);

    float progress = CGame::GetTime()->GetProgress();
    bool isNight = (progress >= 0.30f && progress < 0.90f);

    // 特定のブロックに当たったか判定するため、ブロックマネージャーを取得する
    CBlockManager* pBlockManager = CGame::GetBlockManager();

    bool playerInGrass = pBlockManager->IsPlayerInGrass();
    bool playerInTorch = pBlockManager->IsPlayerInTorch();
    bool playerInWater = pBlockManager->IsPlayerInWater();

    // クールタイム減算
    m_cautionCooldown = std::max(0, m_cautionCooldown - 1);

    // クールタイム減算
    m_afterAttackCooldown = std::max(0, m_afterAttackCooldown - 1);

    // --- 夜にプレイヤーが灯籠 または 水に入ったら ---
    if (isNight && (playerInTorch || playerInWater))
    {
        // 溜め状態
        pEnemy->SetRequestedAction(CEnemy::AI_ACCUMULATE);
        return;
    }

    // --- 夜にプレイヤー草むらに隠れている場合 ---
    if (isNight && playerInGrass && m_cautionCooldown <= 0)
    {
        // 警戒率
        float cautionRate = 0.1f + std::min(0.8f, m_log.inGrassCount / 10.0f);

        int r = rand() % 100;

        if (r < (int)(cautionRate * 100))
        {
            // 警戒行動（近づかない）
            pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_CAUTION);
            m_cautionCooldown = 360;
        }

        return;
    }

    if (distance < 180.0f)
    {
        // プレイヤーが攻撃してきた回数による回避確率
        float evadeRate = std::min(0.5f, m_log.attackCountEvening * 0.05f);
        
        int r = rand() % 100;
        if (r < (int)(evadeRate * 100))
        {
            // 回避状態
            pEnemy->SetRequestedAction(CEnemy::AI_EVADE);
        }
        else
        {
            // 近距離攻撃
            pEnemy->SetRequestedAction(CEnemy::AI_CLOSE_ATTACK_01);
        }
    }
    else if (distance < 250.0f)
    {
        if (m_afterAttackCooldown <= 0)
        {
            int r = rand() % 100;
            if (r < 30)
            {
                // 溜め状態
                pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_ACCUMULATE);
            }
            else
            {
                // 待機状態
                pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_NEUTRAL);
            }
        }
        else
        {
            // クールタイム中は待機状態
            pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_NEUTRAL);
        }
    }
    else
    {
        // 移動状態
        pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_MOVE);
    }
}
//=============================================================================
// プレイヤーの行動記録処理
//=============================================================================
void CEnemyAI::RecordPlayerAction(void)
{
    float progress = CGame::GetTime()->GetProgress(); // 0.0～0.1
    CPlayer* pPlayer = CGame::GetPlayer();

    bool isInGrass = CGame::GetBlockManager()->IsPlayerInGrass();

    bool isAttacking = pPlayer->GetMotion()->IsCurrentMotion(CPlayer::ATTACK_01);

    // 攻撃開始した瞬間だけカウント
    if (isAttacking && !m_prevAttackState)
    {
        m_log.attackCountEvening += 2;
    }

    // 次のフレームのために記録
    m_prevAttackState = isAttacking;

    // 夜 : 草むらに潜む
    if (progress < 0.30f && progress >= 0.90f)
    {
        if (isInGrass)
        {
            m_timer++;// フレーム加算

            if (m_timer <= 70)
            {
                return;
            }

            // 草カウントを加算
            m_log.inGrassCount++;

            m_timer = 0;
        }
        else
        {
            m_timer = 0;
        }
    }
}
