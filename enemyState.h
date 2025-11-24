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
class CEnemy_CloseAttackState1;
class CEnemy_CloseAttackState2;
class CEnemy_CautionState;
class CEnemy_EvadeState;
class CEnemy_GuardState;

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

		case CEnemy::EEnemyAction::AI_CLOSE_ATTACK_01:
			m_pMachine->ChangeState<CEnemy_CloseAttackState1>();
			break;

		case CEnemy::EEnemyAction::AI_CAUTION:
			m_pMachine->ChangeState<CEnemy_CautionState>();
			break;

		case CEnemy::EEnemyAction::AI_EVADE:
			m_pMachine->ChangeState<CEnemy_EvadeState>();
			break;

		case CEnemy::EEnemyAction::AI_GUARD:
			m_pMachine->ChangeState<CEnemy_GuardState>();
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
		// 移動モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::MOVE, 10);
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
		//case CEnemy::EEnemyAction::AI_CLOSE_ATTACK_01:
		//	m_pMachine->ChangeState<CEnemy_CloseAttackState1>();
		//	break;
		case CEnemy::EEnemyAction::AI_CAUTION:
			m_pMachine->ChangeState<CEnemy_CautionState>();
			break;
		case CEnemy::EEnemyAction::AI_EVADE:
			m_pMachine->ChangeState<CEnemy_EvadeState>();
			break;
		case CEnemy::EEnemyAction::AI_GUARD:
			m_pMachine->ChangeState<CEnemy_GuardState>();
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
			{// HPが50以下になったら
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

		float dashPower = 300.0f;// スライドパワー

		D3DXVECTOR3 move = dir * dashPower;

		// 現在の移動量に上書き
		pEnemy->SetMove(move);

		// 物理速度にも即反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(move.y * -10.0f);// 段差で飛ばないように上の力を下げる
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

			float forwardPower = 50.0f; // 滑る速度
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
		velocity.setY(move.y * -10.0f);
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

	void OnExit(CEnemy* pEnemy)override
	{
		// 3秒のクールダウン
		pEnemy->GetAI()->SetAccumulateCooldown(3);
	}

private:

};

//*****************************************************************************
// 近距離攻撃状態1
//*****************************************************************************
class CEnemy_CloseAttackState1 :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 近距離攻撃モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::CLOSE_ATTACK_01, 10);

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

		float dashPower = 20.0f;// スライドパワー

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

			float forwardPower = 10.0f; // 滑る速度
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
			if (pEnemy->GetMotion()->IsAttacking(CEnemy::CLOSE_ATTACK_01, 2, 4, 0, 15))
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

		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::CLOSE_ATTACK_01))
		{// 近距離攻撃モーションが終わっていたら

			// 近距離攻撃状態2
			m_pMachine->ChangeState<CEnemy_CloseAttackState2>();

			return;
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

//*****************************************************************************
// 近距離攻撃状態2
//*****************************************************************************
class CEnemy_CloseAttackState2 :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 近距離攻撃モーション2
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::CLOSE_ATTACK_02, 10);

		// プレイヤー取得
		CPlayer* pPlayer = CGame::GetPlayer();

		// プレイヤーへの方向ベクトル
		D3DXVECTOR3 toPlayer = pPlayer->GetPos() - pEnemy->GetPos();

		// 向いている方向にスライドさせるため、プレイヤー方向ベクトルを取得
		D3DXVECTOR3 dir = toPlayer;

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float dashPower = 20.0f;// スライドパワー

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

			float forwardPower = 10.0f; // 滑る速度
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
			if (pEnemy->GetMotion()->IsAttacking(CEnemy::CLOSE_ATTACK_02, 0, 2, 0, 15))
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

		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::CLOSE_ATTACK_02))
		{// 近距離攻撃モーションが終わっていたら

			// AIリクエストによるステート遷移
			switch (pEnemy->GetRequestedAction())
			{
			case CEnemy::EEnemyAction::AI_NEUTRAL:
				pEnemy->SetRequestedAction(CEnemy::EEnemyAction::AI_NEUTRAL);
				m_pMachine->ChangeState<CEnemy_StandState>();
				break;
			case CEnemy::EEnemyAction::AI_MOVE:
				m_pMachine->ChangeState<CEnemy_MoveState>();
				break;
			case CEnemy::EEnemyAction::AI_ACCUMULATE:
				m_pMachine->ChangeState<CEnemy_AcuumulationState>();
				break;
			case CEnemy::EEnemyAction::AI_CAUTION:
				m_pMachine->ChangeState<CEnemy_CautionState>();
				break;
			case CEnemy::EEnemyAction::AI_EVADE:
				m_pMachine->ChangeState<CEnemy_EvadeState>();
				break;
			case CEnemy::EEnemyAction::AI_GUARD:
				m_pMachine->ChangeState<CEnemy_GuardState>();
				break;
			default:
				break;
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

		float backPower = 13.0f;// スライドパワー

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

		float backPower = 13.0f;// スライドパワー

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
		// ランダムで 1～3 回スライドする
		m_targetSlideCount = 1 + (rand() % 3);
		m_slideCount = 0;

		StartSlide(pEnemy);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		// モーションの進行度を取得
		float motionRate = pEnemy->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pEnemy->GetMove();

		// モーション前半だけ前方移動を維持
		if (motionRate < 0.2f)
		{
			D3DXVECTOR3 forwardDir = m_slideDir;
			D3DXVec3Normalize(&forwardDir, &forwardDir);

			float forwardPower = 20.0f; // 滑る速度
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
		velocity.setY(move.y * -10.0f);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		float targetYaw = atan2f(-m_slideDir.x, -m_slideDir.z);
		D3DXVECTOR3 rotDest = pEnemy->GetRot();
		rotDest.y = targetYaw;

		pEnemy->SetRotDest(rotDest);

		// 向きの補間
		pEnemy->UpdateRotation(0.1f); // 補間速度を少し小さくして自然に

		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::SLIDEMOVE))
		{
			m_slideCount++;

			if (m_slideCount >= m_targetSlideCount)
			{
				// スライド回数終了 → 攻撃状態へ
				m_pMachine->ChangeState<CEnemy_AttackState1>();
			}
			else
			{
				// 次のスライド
				StartSlide(pEnemy);
			}
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:
	int m_slideCount;        // 現在のスライド回数
	int m_targetSlideCount;  // ランダムに決めたスライド回数
	D3DXVECTOR3 m_slideDir; // 今回のスライド方向

	void StartSlide(CEnemy* pEnemy)
	{
		// スライドモーション開始
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::SLIDEMOVE, 10);

		// プレイヤー方向
		CPlayer* pPlayer = CGame::GetPlayer();
		D3DXVECTOR3 toPlayer = pPlayer->GetPos() - pEnemy->GetPos();
		toPlayer.y = 0.0f;

		D3DXVec3Normalize(&toPlayer, &toPlayer);

		// 向いている方向
		D3DXVECTOR3 dir = pEnemy->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		// ランダム角度
		float angleDeg = (float)((rand() % 91) - 45); // ±45度
		float angleRad = angleDeg * (D3DX_PI / 180.0f);

		float cosA = cosf(angleRad);
		float sinA = sinf(angleRad);

		// Y軸回転
		// プレイヤー方向ベースに少しランダムオフセット
		m_slideDir.x = toPlayer.x * cosA - toPlayer.z * sinA;
		m_slideDir.y = 0.0f;
		m_slideDir.z = toPlayer.x * sinA + toPlayer.z * cosA;

		// 正規化
		D3DXVec3Normalize(&m_slideDir, &m_slideDir);

		float slidePower = 300.0f; // スライドパワー
		D3DXVECTOR3 move = m_slideDir * slidePower;

		pEnemy->SetMove(move);

		// 物理速度にも反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(move.y * -10.0f);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);
	}
};

//*****************************************************************************
// 警戒状態
//*****************************************************************************
class CEnemy_CautionState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 警戒モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::CAUTION, 20);
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

		// 灯籠に近づいたら
		if (CGame::GetBlockManager()->IsPlayerInTorch())
		{
			// 溜め状態
			m_pMachine->ChangeState<CEnemy_AcuumulationState>();
			return;
		}

		// 警戒中に攻撃を受けたら
		if (pEnemy->GetWeaponCollider()->IsHit())
		{
			// 回避状態
			m_pMachine->ChangeState<CEnemy_EvadeState>();
			return;
		}

		// 警戒モーションが終わっていたら
		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::CAUTION))
		{
			// 待機状態
			m_pMachine->ChangeState<CEnemy_StandState>();
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

//*****************************************************************************
// 回避状態
//*****************************************************************************
class CEnemy_EvadeState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		StartSlide(pEnemy);

		CPlayer* pPlayer = CGame::GetPlayer();

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
		pEnemy->UpdateRotation(0.5f);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		// モーションの進行度を取得
		float motionRate = pEnemy->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pEnemy->GetMove();

		// モーション前半だけ前方移動を維持
		if (motionRate < 0.2f)
		{
			D3DXVECTOR3 forwardDir = m_slideDir;
			D3DXVec3Normalize(&forwardDir, &forwardDir);

			float forwardPower = 60.0f; // 滑る速度
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
		velocity.setY(move.y * -10.0f);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::EVADE))
		{
			// 待機状態
			m_pMachine->ChangeState<CEnemy_StandState>();
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:
	D3DXVECTOR3 m_slideDir; // 今回のスライド方向

	void StartSlide(CEnemy* pEnemy)
	{
		// 回避モーション開始
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::EVADE, 10);

		// プレイヤー方向
		CPlayer* pPlayer = CGame::GetPlayer();
		D3DXVECTOR3 playerForward = pPlayer->GetForward();
		playerForward.y = 0.0f;
		D3DXVec3Normalize(&playerForward, &playerForward);

		// 向いている方向
		D3DXVECTOR3 dir = pEnemy->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		// ランダム角度（±45度）
		float angleDeg = (float)((rand() % 91) - 45);
		float angleRad = angleDeg * (D3DX_PI / 180.0f);

		float cosA = cosf(angleRad);
		float sinA = sinf(angleRad);

		// プレイヤーの forward を Y軸回転
		m_slideDir.x = playerForward.x * cosA - playerForward.z * sinA;
		m_slideDir.y = 0.0f;
		m_slideDir.z = playerForward.x * sinA + playerForward.z * cosA;

		D3DXVec3Normalize(&m_slideDir, &m_slideDir);

		// スライド移動
		float slidePower = 180.0f;
		D3DXVECTOR3 move = m_slideDir * slidePower;

		pEnemy->SetMove(move);

		// 物理速度にも反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(move.y *  -10.0f);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);
	}
};

//*****************************************************************************
// ガード状態
//*****************************************************************************
class CEnemy_GuardState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// ガードモーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::GUARD, 10);

		// 向いている方向の後ろ
		D3DXVECTOR3 dir = -pEnemy->GetForward();

		// 正規化
		D3DXVec3Normalize(&dir, &dir);

		float backPower = 13.0f;// スライドパワー

		D3DXVECTOR3 move = dir * backPower;

		// 現在の移動量に上書き
		pEnemy->SetMove(move);

		// 物理速度にも即反映
		btVector3 velocity = pEnemy->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setZ(move.z);
		pEnemy->GetRigidBody()->setLinearVelocity(velocity);

		// プレイヤーへの方向ベクトル
		D3DXVECTOR3 toPlayer = CGame::GetPlayer()->GetPos() - pEnemy->GetPos();

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

		// モーション終了後に攻撃
		if (pEnemy->GetMotion()->IsCurrentMotionEnd(CEnemy::GUARD))
		{
			// 待機状態
			m_pMachine->ChangeState<CEnemy_AttackState1>();
		}
	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

#endif
