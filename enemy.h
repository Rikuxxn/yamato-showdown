//=============================================================================
//
// 敵の処理 [enemy.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _ENEMY_H_
#define _ENEMY_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "shadowS.h"
#include "effect.h"
#include "state.h"
#include "manager.h"
#include "debugproc3D.h"
#include "charactermanager.h"
#include "weaponcollider.h"
#include "game.h"
#include "enemyAI.h"
#include "player.h"


//*****************************************************************************
// 敵クラス
//*****************************************************************************
class CEnemy : public CCharacter
{
public:
	CEnemy();
	~CEnemy();

	static constexpr float SPEED = 50.0f;				// 移動スピード

	// 敵モーションの種類
	typedef enum
	{
		NEUTRAL = 0,		// 待機
		MOVE,				// 移動
		ACCUMULATION,		// 溜め
		ATTACK_01,			// 攻撃1(溜めの後)
		//ATTACK_02,			// 攻撃2(近距離)
		DAMAGE,
		DEATH,
		MAX
	}ENEMY_MOTION;

	// AIが要求する行動タイプ
	typedef enum
	{
		ACTION_NONE,
		AI_NEUTRAL,
		AI_MOVE,
		AI_ACCUMULATE,
		AI_ATTACK_01,
		AI_ATTACK_02,
		AI_MAX
	}EEnemyAction;

	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetRequestedAction(EEnemyAction action) { m_requestedAction = action; }
	void SetIsDeath(bool flag) { m_isDead = flag; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	CMotion* GetMotion(void) { return m_pMotion; }
	CModel* GetWeapon(void) { return m_pSwordModel; }
	CWeaponCollider* GetWeaponCollider(void) { return m_pWeaponCollider.get(); }
	D3DXVECTOR3 GetForward(void);
	CEnemyAI* GetAI(void) { return m_pAI.get(); }
	EEnemyAction GetRequestedAction(void) const { return m_requestedAction; }
	bool IsDeath(void) { return m_isDead; }// 死んだかどうか

	void Damage(float fDamage) override;

private:
	static constexpr int MAX_PARTS = 32;	// 最大パーツ数
	static constexpr float CAPSULE_RADIUS = 14.0f;					// カプセルコライダーの半径
	static constexpr float CAPSULE_HEIGHT = 60.0f;					// カプセルコライダーの高さ

	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	CModel* m_apModel[MAX_PARTS];			// モデル(パーツ)へのポインタ
	CShadowS* m_pShadowS;					// ステンシルシャドウへのポインタ
	CDebugProc3D* m_pDebug3D;				// 3Dデバッグ表示へのポインタ
	CMotion* m_pMotion;						// モーションへのポインタ
	int m_nNumModel;						// モデル(パーツ)の総数
	CModel* m_pSwordModel; // 武器モデルのポインタ
	CObjectX* m_pTipModel;	// 武器コライダー用モデル
	CObjectX* m_pBaseModel;	// 武器コライダー用モデル
	std::unique_ptr<CWeaponCollider> m_pWeaponCollider;// 武器の当たり判定へのポインタ

	std::unique_ptr<CEnemyAI> m_pAI;
	EEnemyAction m_requestedAction;
	bool m_isDead;

	// ステートを管理するクラスのインスタンス
	StateMachine<CEnemy> m_stateMachine;

};

#endif

