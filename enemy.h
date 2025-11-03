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

// 前方宣言
class CEnemy_StandState;
class CEnemy_MoveState;
class CEnemy_AcuumulationState;
class CEnemy_AttackState1;
class CEnemy_AttackState2;
class CEnemy_DamageState;

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

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	CMotion* GetMotion(void) { return m_pMotion; }
	CModel* GetWeapon(void) { return m_pSwordModel; }
	CWeaponCollider* GetWeaponCollider(void) { return m_pWeaponCollider.get(); }
	D3DXVECTOR3 GetForward(void);
	CEnemyAI* GetAI(void) { return m_pAI.get(); }
	EEnemyAction GetRequestedAction(void) const { return m_requestedAction; }

	void Damage(float fDamage) override
	{
		// まず共通のHP処理
		CCharacter::Damage(fDamage);

		// 死亡時
		if (IsDead())
		{
			//// 死亡状態
			//m_stateMachine.ChangeState<CEnemy_DeadState>();
			return;
		}

		// ダメージステートへ
		m_stateMachine.ChangeState<CEnemy_DamageState>();
	}

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

	// ステートを管理するクラスのインスタンス
	StateMachine<CEnemy> m_stateMachine;

};


//*****************************************************************************
// 待機状態
//*****************************************************************************
class CEnemy_StandState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 待機モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::NEUTRAL, 20);

		// ヒットフラグをリセット
		pEnemy->GetWeaponCollider()->ResetHit();
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		D3DXVECTOR3 move = pEnemy->GetMove();

		move *= 0.95f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pEnemy->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);


		switch (pEnemy->GetRequestedAction())
		{
		case CEnemy::EEnemyAction::AI_MOVE:// 移動状態
			m_pMachine->ChangeState<CEnemy_MoveState>();
			break;

		case CEnemy::EEnemyAction::AI_ACCUMULATE:// 溜め状態
			m_pMachine->ChangeState<CEnemy_AcuumulationState>();
			break;

		default:
			break; // そのまま待機
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

//*****************************************************************************
// 移動状態
//*****************************************************************************
class CEnemy_MoveState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		if (!pEnemy->GetMotion()->IsAttacking(CEnemy::ATTACK_01))
		{
			// 移動モーション
			pEnemy->GetMotion()->StartBlendMotion(CEnemy::MOVE, 10);
		}
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		// 目標速度計算
		float moveSpeed = CEnemy::SPEED;
		D3DXVECTOR3 targetMove = pEnemy->GetForward();

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);

			targetMove *= moveSpeed;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// 現在速度との補間（イージング）
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pEnemy->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// 補間後の速度をプレイヤーにセット
		pEnemy->SetMove(currentMove);

		// 物理エンジンにセット
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		// プレイヤー方向に向けるため、プレイヤーを取得する
		CPlayer* pPlayer = CGame::GetPlayer();

		if (!pPlayer)
		{
			return;
		}

		// プレイヤーへの方向ベクトル
		D3DXVECTOR3 toPlayer = pPlayer->GetPos() - pEnemy->GetPos();
		toPlayer.y = 0.0f; // 水平方向のみ
		D3DXVec3Normalize(&toPlayer, &toPlayer);

		// 目標の角度を算出
		float targetYaw = atan2f(-toPlayer.x, -toPlayer.z);

		// 目的角度を設定（X,Zはそのまま）
		D3DXVECTOR3 rotDest = pEnemy->GetRot();
		rotDest.y = targetYaw;

		// 敵に目的角度を設定
		pEnemy->SetRotDest(rotDest);

		// 補間して回転
		pEnemy->UpdateRotation(0.1f);

		// AIリクエストによるステート遷移
		switch (pEnemy->GetRequestedAction())
		{
		case CEnemy::EEnemyAction::AI_NEUTRAL:
			if (!pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_01))
			{
				pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_NEUTRAL);
				m_pMachine->ChangeState<CEnemy_StandState>();
			}
			break;
		case CEnemy::EEnemyAction::AI_ACCUMULATE:
			m_pMachine->ChangeState<CEnemy_AcuumulationState>();
			break;
		default:
			break;
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

//*****************************************************************************
// 溜め状態
//*****************************************************************************
class CEnemy_AcuumulationState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 溜めモーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::ACCUMULATION, 20);

		// プレイヤー取得
		CPlayer* pPlayer = CGame::GetPlayer();

		// プレイヤーへの方向ベクトル
		D3DXVECTOR3 toPlayer = pPlayer->GetPos() - pEnemy->GetPos();

		if (pPlayer)
		{
			toPlayer.y = 0.0f; // 水平方向のみ
			D3DXVec3Normalize(&toPlayer, &toPlayer);

			// 目標の角度を算出
			float targetYaw = atan2f(-toPlayer.x, -toPlayer.z);

			// 目的角度を設定（X,Zはそのまま）
			D3DXVECTOR3 rotDest = pEnemy->GetRot();
			rotDest.y = targetYaw;

			// 敵に目的角度を設定
			pEnemy->SetRotDest(rotDest);

			// 補間して回転
			pEnemy->UpdateRotation(0.1f);
		}
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		D3DXVECTOR3 move = pEnemy->GetMove();

		move *= 0.95f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pEnemy->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::ACCUMULATION))
		{// 溜めモーションが終わっていたら
			// 攻撃状態1
			m_pMachine->ChangeState<CEnemy_AttackState1>();

			return;
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

//*****************************************************************************
// 攻撃状態1
//*****************************************************************************
class CEnemy_AttackState1 :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 攻撃モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::ATTACK_01, 10);

		// プレイヤー取得
		CPlayer* pPlayer = CGame::GetPlayer();

		// プレイヤーへの方向ベクトル
		D3DXVECTOR3 toPlayer = pPlayer->GetPos() - pEnemy->GetPos();

		if (pPlayer)
		{
			toPlayer.y = 0.0f; // 水平方向のみ
			D3DXVec3Normalize(&toPlayer, &toPlayer);

			// 目標の角度を算出
			float targetYaw = atan2f(-toPlayer.x, -toPlayer.z);

			// 目的角度を設定（X,Zはそのまま）
			D3DXVECTOR3 rotDest = pEnemy->GetRot();
			rotDest.y = targetYaw;

			// 敵に目的角度を設定
			pEnemy->SetRotDest(rotDest);

			// 補間して回転
			pEnemy->UpdateRotation(0.1f);
		}

		// 向いている方向にスライドさせるため、プレイヤー方向ベクトルを取得
		D3DXVECTOR3 dir = toPlayer;

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float dashPower = 3800.0f;// スライドパワー

		D3DXVECTOR3 move = dir * dashPower;

		// 現在の移動量に上書き
		pEnemy->SetMove(move);

		// 物理速度にも即反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(-100.3f);// 段差で飛ばないように上の力を下げる
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		D3DXVECTOR3 move = pEnemy->GetMove();

		move *= 0.85f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pEnemy->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		if (pEnemy->GetWeapon() && pEnemy->GetWeaponCollider())
		{
			// 攻撃中だけ有効化
			if (pEnemy->GetMotion()->IsAttacking(CEnemy::ATTACK_01, 1, 2, 0, 30))
			{
				pEnemy->GetWeaponCollider()->SetActive(true);
				pEnemy->GetWeaponCollider()->ResetPrevPos();
			}
			else
			{
				pEnemy->GetWeaponCollider()->SetActive(false);
			}

			// プレイヤーに当たったか判定する
			pEnemy->GetWeaponCollider()->CheckHit(CGame::GetPlayer());
		}

		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::ATTACK_01))
		{// 攻撃モーションが終わっていたら
			// 待機状態
			m_pMachine->ChangeState<CEnemy_StandState>();

			return;
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

//*****************************************************************************
// ダメージ状態
//*****************************************************************************
class CEnemy_DamageState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// ダメージモーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::DAMAGE, 20);

		m_timer = 0.0f;
		m_duration = 30.0f; // ダメージ硬直フレーム数
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		m_timer++;

		D3DXVECTOR3 move = pEnemy->GetMove();

		move *= 0.95f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pEnemy->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		// 一定時間後に待機へ戻す
		if (m_timer > m_duration || pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::DAMAGE))
		{
			// 待機状態
			m_pMachine->ChangeState<CEnemy_StandState>();
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{
		m_verticalVelocity = 0.0f;
	}

private:
	float m_verticalVelocity; // 上下方向速度
	float m_timer;
	float m_duration;
};

#endif

