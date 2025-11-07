//=============================================================================
//
// 敵の状態処理 [enemyState.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#include "particle.h"
#ifndef _ENEMYSTATE_H_
#define _ENEMYSTATE_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


// 前方宣言
class CEnemy_StandState;
class CEnemy_MoveState;
class CEnemy_AcuumulationState;
class CEnemy_AttackState1;
class CEnemy_AttackState2;
class CEnemy_DamageState;
class CEnemy_DeadState;
class CEnemy_SlideMoveState;

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
		D3DXVECTOR3 spawnPos = pEnemy->GetPos();
		spawnPos.y += 30.0f;

		// パーティクル生成
		CParticle::Create<CAuraParticle>(INIT_VEC3, spawnPos, D3DXCOLOR(0.7f, 0.1f, 0.1f, 0.9f), 50, 2);
		CParticle::Create<CAuraParticle>(INIT_VEC3, spawnPos, D3DXCOLOR(0.8f, 0.4f, 0.1f, 0.8f), 50, 1);

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

			if (pEnemy->GetHp() <= 50.0f)
			{
				// スライド移動状態
				m_pMachine->ChangeState<CEnemy_SlideMoveState>();
			}
			else
			{
				// 攻撃状態1
				m_pMachine->ChangeState<CEnemy_AttackState1>();
			}

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
			pEnemy->UpdateRotation(0.5f);
		}

		// 向いている方向にスライドさせるため、プレイヤー方向ベクトルを取得
		D3DXVECTOR3 dir = toPlayer;

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float dashPower = 3000.0f;// スライドパワー

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
		// モーションの進行度を取得
		float motionRate = pEnemy->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pEnemy->GetMove();

		// モーション前半だけ前方移動を維持
		if (motionRate < 0.2f)
		{
			D3DXVECTOR3 forwardDir = pEnemy->GetForward();
			D3DXVec3Normalize(&forwardDir, &forwardDir);

			float forwardPower = 500.0f; // 滑る速度
			move = forwardDir * forwardPower;
		}
		else
		{
			move *= 0.82f; // 減速率
			if (fabsf(move.x) < 0.01f) move.x = 0;
			if (fabsf(move.z) < 0.01f) move.z = 0;
		}

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
			if (pEnemy->GetMotion()->IsAttacking(CEnemy::ATTACK_01, 0, 2, 0, 20))
			{
				pEnemy->GetWeaponCollider()->SetActive(true);
				pEnemy->GetWeaponCollider()->ResetPrevPos();
			}
			else
			{
				pEnemy->GetWeaponCollider()->SetActive(false);
			}

			// プレイヤーに当たったか判定する
			pEnemy->GetWeaponCollider()->CheckHit(CGame::GetPlayer(), 1.0f);
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
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::DAMAGE, 10);

		// 向いている方向の後ろ
		D3DXVECTOR3 dir = -pEnemy->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float backPower = 130.0f;// スライドパワー

		D3DXVECTOR3 move = dir * backPower;

		// 現在の移動量に上書き
		pEnemy->SetMove(move);

		// 物理速度にも即反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		D3DXVECTOR3 move = pEnemy->GetMove();

		move *= 0.90f; // 減速率
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
		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::DAMAGE))
		{
			// 待機状態
			m_pMachine->ChangeState<CEnemy_StandState>();
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:
	float m_verticalVelocity; // 上下方向速度
};


//*****************************************************************************
// 死亡状態
//*****************************************************************************
class CEnemy_DeadState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 死亡モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::DEATH, 10);

		// 向いている方向の後ろ
		D3DXVECTOR3 dir = -pEnemy->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float backPower = 130.0f;// スライドパワー

		D3DXVECTOR3 move = dir * backPower;

		// 現在の移動量に上書き
		pEnemy->SetMove(move);

		// 物理速度にも即反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		D3DXVECTOR3 move = pEnemy->GetMove();

		move *= 0.90f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pEnemy->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		// 死亡モーションが終わったら死亡判定
		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::DEATH))
		{
			// 死亡
			pEnemy->SetIsDeath(true);
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:
};

//*****************************************************************************
// スライド移動状態
//*****************************************************************************
class CEnemy_SlideMoveState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// スライド移動モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::SLIDEMOVE, 10);

		// 向いている方向
		D3DXVECTOR3 dir = pEnemy->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float backPower = 3000.0f;// スライドパワー

		D3DXVECTOR3 move = dir * backPower;

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
		// モーションの進行度を取得
		float motionRate = pEnemy->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pEnemy->GetMove();

		// モーション前半だけ前方移動を維持
		if (motionRate < 0.2f)
		{
			D3DXVECTOR3 forwardDir = pEnemy->GetForward();
			D3DXVec3Normalize(&forwardDir, &forwardDir);

			float forwardPower = 200.0f; // 滑る速度
			move = forwardDir * forwardPower;
		}
		else
		{
			move *= 0.82f; // 減速率
			if (fabsf(move.x) < 0.01f) move.x = 0;
			if (fabsf(move.z) < 0.01f) move.z = 0;
		}

		// 移動量を設定
		pEnemy->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		if (pEnemy->GetMotion()->IsAttacking(CEnemy::SLIDEMOVE, 1, 1, 0, 40))
		//if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::SLIDEMOVE))
		{
			// 攻撃状態1
			m_pMachine->ChangeState<CEnemy_AttackState1>();
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

#endif
