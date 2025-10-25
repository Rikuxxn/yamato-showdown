//=============================================================================
//
// プレイヤー処理 [player.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYER_H_// このマクロ定義がされていなかったら
#define _PLAYER_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"
#include "motion.h"
#include "debugproc3D.h"
#include "block.h"
#include "shadowS.h"
#include "effect.h"
#include "state.h"
#include "manager.h"
#include "charactermanager.h"

// 前方宣言
class CPlayer_StandState;
class CPlayer_MoveState;
class CPlayer_AttackState1;

// 入力データ構造体
struct InputData
{
	D3DXVECTOR3 moveDir;      // 前後移動ベクトル
	bool attack;              // 攻撃入力
};

//*****************************************************************************
// プレイヤークラス
//*****************************************************************************
class CPlayer : public CCharacter
{
public:
	CPlayer();
	~CPlayer();

	static constexpr float PLAYER_SPEED = 200.0f;				// 移動スピード

	// プレイヤーモーションの種類
	typedef enum
	{
		NEUTRAL = 0,		// 待機
		MOVE,				// 移動
		ATTACK_01,			// 攻撃1
		MAX
	}PLAYER_MOTION;

	static CPlayer* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	CMotion* GetMotion(void) { return m_pMotion; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	bool GetPlayerUse(void) const { return m_playerUse; }
	bool GetOnGround(void) { return m_bOnGround; }
	bool GetIsMoving(void) const { return m_bIsMoving; }
	D3DXVECTOR3 GetForward(void);
	InputData GatherInput(void);
	CBlock* FindFrontBlockByRaycast(float rayLength);

	// ステート用にフラグ更新
	void UpdateMovementFlags(const D3DXVECTOR3& moveDir)
	{
		m_bIsMoving = (moveDir.x != 0.0f || moveDir.z != 0.0f);
	}

private:
	static constexpr int MAX_PARTS			= 32;		// 最大パーツ数
	static constexpr float MAX_GRAVITY		= -0.26f;	// 重力加速度
	static constexpr float CAPSULE_RADIUS	= 10.5f;	// カプセルコライダーの半径
	static constexpr float CAPSULE_HEIGHT	= 45.5f;	// カプセルコライダーの高さ

	D3DXVECTOR3 m_move;					// 移動量
	D3DXMATRIX m_mtxWorld;				// ワールドマトリックス
	CModel* m_apModel[MAX_PARTS];		// モデル(パーツ)へのポインタ
	CShadowS* m_pShadowS;				// ステンシルシャドウへのポインタ
	CMotion* m_pMotion;					// モーションへのポインタ
	CDebugProc3D* m_pDebug3D;			// 3Dデバッグ表示へのポインタ
	int m_nNumModel;					// モデル(パーツ)の総数
	bool m_playerUse;					// 使われているかどうか
	bool m_bIsMoving;					// 移動入力フラグ
	bool m_bOnGround;					// 接地フラグ
	int m_particleTimer;				// パーティクルタイマー
	static constexpr int DASH_PARTICLE_INTERVAL = 10; // パーティクル発生間隔（フレーム数）

	// ステートを管理するクラスのインスタンス
	StateMachine<CPlayer> m_stateMachine;
};


//*****************************************************************************
// プレイヤーの待機状態
//*****************************************************************************
class CPlayer_StandState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{		
		// 待機モーション
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::NEUTRAL, 10);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力を取得
		InputData input = pPlayer->GatherInput();

		// 攻撃入力があれば攻撃ステート1に移行
		if (input.attack && !pPlayer->GetMotion()->IsAttacking())
		{
			// 攻撃状態1へ移行
			m_pMachine->ChangeState<CPlayer_AttackState1>();
			return;
		}

		// 移動入力があれば移動ステートに移行
		if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
		{
			// 移動状態へ移行
			m_pMachine->ChangeState<CPlayer_MoveState>();
		}

		D3DXVECTOR3 move = pPlayer->GetMove();

		move *= 0.85f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

//*****************************************************************************
// プレイヤーの移動状態
//*****************************************************************************
class CPlayer_MoveState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		if (!pPlayer->GetMotion()->IsAttacking())
		{
			// 移動モーション
			pPlayer->GetMotion()->StartBlendMotion(CPlayer::MOVE, 10);
		}
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力取得
		InputData input = pPlayer->GatherInput();

		// フラグ更新
		pPlayer->UpdateMovementFlags(input.moveDir);

		// 攻撃入力があれば攻撃ステート1に移行
		if (input.attack)
		{
			// 攻撃状態1
			m_pMachine->ChangeState<CPlayer_AttackState1>();
			return;
		}

		// 目標速度計算
		float moveSpeed = CPlayer::PLAYER_SPEED;

		D3DXVECTOR3 targetMove = input.moveDir;

		if (targetMove.x != 0.0f || targetMove.z != 0.0f)
		{
			D3DXVec3Normalize(&targetMove, &targetMove);
			moveSpeed = CPlayer::PLAYER_SPEED;

			targetMove *= moveSpeed;
		}
		else
		{
			targetMove = D3DXVECTOR3(0, 0, 0);
		}

		// 現在速度との補間（イージング）
		const float accelRate = 0.15f;
		D3DXVECTOR3 currentMove = pPlayer->GetMove();

		currentMove.x += (targetMove.x - currentMove.x) * accelRate;
		currentMove.z += (targetMove.z - currentMove.z) * accelRate;

		// 補間後の速度をプレイヤーにセット
		pPlayer->SetMove(currentMove);

		// 物理エンジンにセット
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(currentMove.x);
		velocity.setZ(currentMove.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		// 移動していなければ待機ステートに戻す
		if (!pPlayer->GetIsMoving() && !pPlayer->GetMotion()->IsAttacking())
		{
			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

//*****************************************************************************
// プレイヤーの攻撃状態1
//*****************************************************************************
class CPlayer_AttackState1 :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// 攻撃モーション
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::ATTACK_01, 10);

		// 向いている方向にスライドさせるため、プレイヤーの前方ベクトルを取得
		D3DXVECTOR3 dir = pPlayer->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float dashPower = 800.0f;// スライドパワー

		D3DXVECTOR3 move = dir * dashPower;

		// 現在の移動量に上書き
		pPlayer->SetMove(move);

		// 物理速度にも即反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力取得
		InputData input = pPlayer->GatherInput();

		// フラグ更新
		pPlayer->UpdateMovementFlags(input.moveDir);

		D3DXVECTOR3 move = pPlayer->GetMove();

		move *= 0.85f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (pPlayer->GetMotion()->IsCurrentMotionEnd(CPlayer::ATTACK_01))
		{// 攻撃モーションが終わっていたら
			// 待機状態
			pPlayer->GetMotion()->SetMotion(CPlayer::NEUTRAL);
			m_pMachine->ChangeState<CPlayer_StandState>();

			return;
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:

};

#endif
