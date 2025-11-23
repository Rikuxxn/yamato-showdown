//=============================================================================
//
// 敵の学習AI処理 [enemyAI.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _ENEMYAI_H_
#define _ENEMYAI_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


// --- 前方宣言 ---
class CPlayer;
class CEnemy;

//*****************************************************************************
// 敵AIクラス
//*****************************************************************************
class CEnemyAI
{
public:
	CEnemyAI();
	~CEnemyAI();

	// プレイヤーの行動記録構造体
	struct PlayerBehaviorLog
	{
		int attackCountEvening = 0;
		int inGrassCount = 0;// 草むらに潜む
	};

	void Update(CEnemy* pEnemy, CPlayer* pPlayer);
	void RecordPlayerAction(CEnemy* pEnemy, CPlayer* pPlayer);
	void SetAccumulateCooldown(int nTime) { m_afterAttackCooldown = nTime * 60; }

private:
	int m_timer;
	int m_afterAttackCooldown;
	int m_cautionCooldown;
	PlayerBehaviorLog m_log;
	bool m_prevAttackState;
};

#endif

