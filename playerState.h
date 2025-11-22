//=============================================================================
//
// プレイヤーの状態処理 [playerState.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PLAYERSTATE_H_// このマクロ定義がされていなかったら
#define _PLAYERSTATE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************


// 前方宣言
class CPlayer_StandState;
class CPlayer_MoveState;
class CPlayer_AttackState1;
class CPlayer_BackfripState;
class CPlayer_JumpSlashState;
class CPlayer_DamageState;

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

		// ヒットフラグをリセット
		pPlayer->GetWeaponCollider()->ResetHit();
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力を取得
		InputData input = pPlayer->GatherInput();

		// 攻撃入力があれば攻撃ステート1に移行
		if (input.attack && !pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_01))
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

		move *= 0.82f; // 減速率
		if (fabsf(move.x) < 0.01f) move.x = 0;
		if (fabsf(move.z) < 0.01f) move.z = 0;

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(-50.0f);
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
		if (!pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_01))
		{
			// 移動モーション
			pPlayer->GetMotion()->StartBlendMotion(CPlayer::MOVE, 10);
		}

		m_particleTimer = 0;
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

		CModelEffect* pModelEffect = nullptr;

		if (pPlayer->GetIsMoving() && pPlayer->GetOnGround())
		{
			m_particleTimer++;

			if (m_particleTimer >= DASH_PARTICLE_INTERVAL)
			{
				m_particleTimer = 0;

				// ランダムな角度で横に広がる
				float angle = ((rand() % 360) / 180.0f) * D3DX_PI;
				float speed = (rand() % 150) / 300.0f + 0.2f;

				// 移動量
				D3DXVECTOR3 move;
				move.x = cosf(angle) * speed;
				move.z = sinf(angle) * speed;
				move.y = (rand() % 80) / 50.0f + 0.05f; // 少しだけ上方向

				// 向き
				D3DXVECTOR3 rot;
				rot.x = ((rand() % 360) / 180.0f) * D3DX_PI;
				rot.y = ((rand() % 360) / 180.0f) * D3DX_PI;
				rot.z = ((rand() % 360) / 180.0f) * D3DX_PI;

				// モデルエフェクトの生成
				pModelEffect = CModelEffect::Create("data/MODELS/effectModel_step.x", pPlayer->GetPos(), rot,
					move, D3DXVECTOR3(0.3f, 0.3f, 0.3f), 180, 0.01f, 0.008f);
			}
		}
		else
		{
			m_particleTimer = 0; // 停止時はリセット
		}

		// 移動していなければ待機ステートに戻す
		if (!pPlayer->GetIsMoving() && !pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_01))
		{
			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:
	static constexpr int DASH_PARTICLE_INTERVAL = 10; // パーティクル発生間隔（フレーム数）
	int m_particleTimer;
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

		// 敵取得
		CEnemy* pEnemy = CGame::GetEnemy();

		// 敵への方向ベクトル
		D3DXVECTOR3 toEnemy = pEnemy->GetPos() - pPlayer->GetPos();

		// 対象との距離を求める
		float distance = D3DXVec3Length(&toEnemy);

		// 範囲内に入ったら
		if (pEnemy && distance < TRIGGER_DISTANCE)
		{
			toEnemy.y = 0.0f; // 水平方向のみ
			D3DXVec3Normalize(&toEnemy, &toEnemy);

			// 目標の角度を算出
			float targetYaw = atan2f(-toEnemy.x, -toEnemy.z);

			// 目的角度を設定（X,Zはそのまま）
			D3DXVECTOR3 rotDest = pPlayer->GetRot();
			rotDest.y = targetYaw;

			// 敵に目的角度を設定
			pPlayer->SetRotDest(rotDest);

			// 補間して回転
			pPlayer->UpdateRotation(0.5f);
		}
		else
		{
			// 範囲外だったら前方向にスライド
			toEnemy = pPlayer->GetForward();
		}

		float dashPower = 80.0f;// スライドパワー

		D3DXVECTOR3 move = toEnemy * dashPower;

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

		if (pPlayer->GetWeapon() && pPlayer->GetWeaponCollider())
		{
			// 攻撃中だけ有効化
			if (pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_01, 2, 3, 0, 5))
			{
				pPlayer->GetWeaponCollider()->SetActive(true);
				pPlayer->GetWeaponCollider()->ResetPrevPos();
			}
			else
			{
				pPlayer->GetWeaponCollider()->SetActive(false);
			}

			// 敵に当たったか判定する
			pPlayer->GetWeaponCollider()->CheckHit(CGame::GetEnemy(), 3.0f);
		}

		if (pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_01))
		{// 攻撃モーションの途中で入力があった場合
			if (input.attack)
			{
				// バク転状態
				m_pMachine->ChangeState<CPlayer_BackfripState>();
			}
		}

		if (pPlayer->GetMotion()->IsCurrentMotionEnd(CPlayer::ATTACK_01))
		{// 攻撃モーションが終わっていたら

			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();

			return;
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{

	}

private:
	static constexpr float TRIGGER_DISTANCE = 250.0f;
};

//*****************************************************************************
// プレイヤーのバク転状態
//*****************************************************************************
class CPlayer_BackfripState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// バク転モーション
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::BACKFRIP, 10);

		// ヒットフラグをリセット
		pPlayer->GetWeaponCollider()->ResetHit();

		CEnemy* pEnemy = CGame::GetEnemy();

		// 敵への方向ベクトル
		D3DXVECTOR3 toEnemy = pEnemy->GetPos() - pPlayer->GetPos();

		// 対象との距離を求める
		float distance = D3DXVec3Length(&toEnemy);

		// 範囲内に入ったら
		if (pEnemy && distance < TRIGGER_DISTANCE)
		{
			toEnemy.y = 0.0f; // 水平方向のみ
			D3DXVec3Normalize(&toEnemy, &toEnemy);

			// 目標の角度を算出
			float targetYaw = atan2f(-toEnemy.x, -toEnemy.z);

			// 目的角度を設定（X,Zはそのまま）
			D3DXVECTOR3 rotDest = pPlayer->GetRot();
			rotDest.y = targetYaw;

			// 敵に目的角度を設定
			pPlayer->SetRotDest(rotDest);

			// 補間して回転
			pPlayer->UpdateRotation(0.1f);
		}
		else
		{
			// 範囲外だったら後ろ方向にスライド
			toEnemy = -pPlayer->GetForward();
		}

		float backPower = 350.0f; // 初速パワー
		D3DXVECTOR3 move = -toEnemy * backPower;

		pPlayer->SetMove(move);

		// 上方向初速
		m_verticalVelocity = 40.0f;

		// 物理に反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(m_verticalVelocity);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// 入力取得
		InputData input = pPlayer->GatherInput();

		// フラグ更新
		pPlayer->UpdateMovementFlags(input.moveDir);

		// バク転モーションの進行度を取得
		float motionRate = pPlayer->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pPlayer->GetMove();

		// モーション前半だけ後方移動を維持
		if (motionRate < 0.4f)
		{
			D3DXVECTOR3 backDir = -pPlayer->GetForward();
			D3DXVec3Normalize(&backDir, &backDir);

			float backPower = 40.0f; // 滑りながら下がる速度
			move = backDir * backPower;
		}
		else
		{
			// 後半は減速
			move *= 0.82f;
			if (fabsf(move.x) < 0.01f) move.x = 0;
			if (fabsf(move.z) < 0.01f) move.z = 0;
		}

		// ===== 重力処理 =====
		m_verticalVelocity -= 2.0f; // 重力加速度
		if (m_verticalVelocity < -30.0f)
		{
			m_verticalVelocity = -30.0f; // 最大落下速度を制限
		}

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(m_verticalVelocity);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (motionRate > 0.5f)
		{
			if (input.attack)
			{
				// ジャンプ斬り攻撃状態
				m_pMachine->ChangeState<CPlayer_JumpSlashState>();
				return;
			}
		}

		// バク転モーションが終わったら待機ステートに戻す
		if (pPlayer->GetMotion()->IsCurrentMotionEnd(CPlayer::BACKFRIP))
		{
			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{
		m_verticalVelocity = 0.0f;
	}

private:
	static constexpr float TRIGGER_DISTANCE = 250.0f;

	float m_verticalVelocity = 0.0f; // 上下方向速度
};

//*****************************************************************************
// ジャンプ斬り攻撃状態
//*****************************************************************************
class CPlayer_JumpSlashState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// 攻撃モーション
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::ATTACK_JUMPSLASH, 10);

		// 敵取得
		CEnemy* pEnemy = CGame::GetEnemy();

		// 敵への方向ベクトル
		D3DXVECTOR3 toEnemy = pEnemy->GetPos() - pPlayer->GetPos();

		// 対象との距離を求める
		float distance = D3DXVec3Length(&toEnemy);

		// 範囲内に入ったら
		if (pEnemy && distance < TRIGGER_DISTANCE)
		{
			toEnemy.y = 0.0f; // 水平方向のみ
			D3DXVec3Normalize(&toEnemy, &toEnemy);

			// 目標の角度を算出
			float targetYaw = atan2f(-toEnemy.x, -toEnemy.z);

			// 目的角度を設定（X,Zはそのまま）
			D3DXVECTOR3 rotDest = pPlayer->GetRot();
			rotDest.y = targetYaw;

			// 敵に目的角度を設定
			pPlayer->SetRotDest(rotDest);

			// 補間して回転
			pPlayer->UpdateRotation(0.1f);
		}
		else
		{
			// 範囲外だったら前方向にスライド
			toEnemy = pPlayer->GetForward();
		}

		float forwardPower = 200.0f; // 初速パワー
		D3DXVECTOR3 move = toEnemy * forwardPower;

		pPlayer->SetMove(move);

		// 上方向初速
		m_verticalVelocity = 55.0f;

		// 物理に反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(m_verticalVelocity);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// モーションの進行度を取得
		float motionRate = pPlayer->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pPlayer->GetMove();

		// モーション前半だけ前方移動を維持
		if (motionRate < 0.4f)
		{
			D3DXVECTOR3 forwardDir = pPlayer->GetForward();
			D3DXVec3Normalize(&forwardDir, &forwardDir);

			float forwardPower = 20.0f; // 滑る速度
			move = forwardDir * forwardPower;
		}
		else
		{
			// 後半は減速
			move *= 0.82f;
			if (fabsf(move.x) < 0.01f) move.x = 0;
			if (fabsf(move.z) < 0.01f) move.z = 0;
		}

		// ===== 重力処理 =====
		m_verticalVelocity -= 3.0f; // 重力加速度
		if (m_verticalVelocity < -30.0f)
		{
			m_verticalVelocity = -30.0f; // 最大落下速度を制限
		}

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(m_verticalVelocity);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (pPlayer->GetWeapon() && pPlayer->GetWeaponCollider())
		{
			// 攻撃中だけ有効化
			if (pPlayer->GetMotion()->IsAttacking(CPlayer::ATTACK_JUMPSLASH, 2, 3, 0, 10))
			{
				pPlayer->GetWeaponCollider()->SetActive(true);
				pPlayer->GetWeaponCollider()->ResetPrevPos();
			}
			else
			{
				pPlayer->GetWeaponCollider()->SetActive(false);
			}

			// 敵に当たったか判定する
			pPlayer->GetWeaponCollider()->CheckHit(CGame::GetEnemy(), 6.0f);
		}

		if (pPlayer->GetMotion()->IsCurrentMotionEnd(CPlayer::ATTACK_JUMPSLASH))
		{// 攻撃モーションが終わっていたら
			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();

			return;
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{
		m_verticalVelocity = 0.0f;
	}

private:
	static constexpr float TRIGGER_DISTANCE = 300.0f;

	float m_verticalVelocity = 0.0f; // 上下方向速度
};

//*****************************************************************************
// プレイヤーのダメージ状態
//*****************************************************************************
class CPlayer_DamageState :public StateBase<CPlayer>
{
public:

	void OnStart(CPlayer* pPlayer)override
	{
		// ダメージモーション
		pPlayer->GetMotion()->StartBlendMotion(CPlayer::DAMAGE, 10);

		// 最初の勢い（後方への初速）
		D3DXVECTOR3 damageDir = CGame::GetEnemy()->GetForward();
		D3DXVec3Normalize(&damageDir, &damageDir);

		float forwardPower = 120.0f; // 初速パワー
		D3DXVECTOR3 move = damageDir * forwardPower;

		pPlayer->SetMove(move);

		// 上方向初速
		m_verticalVelocity = 50.0f;

		// 物理に反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(m_verticalVelocity);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);
	}

	void OnUpdate(CPlayer* pPlayer)override
	{
		// モーションの進行度を取得
		float motionRate = pPlayer->GetMotion()->GetMotionRate(); // 0.0～1.0
		D3DXVECTOR3 move = pPlayer->GetMove();

		// モーション前半だけ前方移動を維持
		if (motionRate < 0.4f)
		{
			D3DXVECTOR3 forwardDir = -pPlayer->GetForward();
			D3DXVec3Normalize(&forwardDir, &forwardDir);

			float forwardPower = 15.0f; // 滑る速度
			move = forwardDir * forwardPower;
		}
		else
		{
			// 後半は減速
			move *= 0.88f;
			if (fabsf(move.x) < 0.01f) move.x = 0;
			if (fabsf(move.z) < 0.01f) move.z = 0;
		}

		// ===== 重力処理 =====
		m_verticalVelocity -= 3.0f; // 重力加速度
		if (m_verticalVelocity < -30.0f)
		{
			m_verticalVelocity = -30.0f; // 最大落下速度を制限
		}

		// 移動量を設定
		pPlayer->SetMove(move);

		// リジッドボディに反映
		btVector3 velocity = pPlayer->GetRigidBody()->getLinearVelocity();
		velocity.setX(move.x);
		velocity.setY(m_verticalVelocity);
		velocity.setZ(move.z);
		pPlayer->GetRigidBody()->setLinearVelocity(velocity);

		if (pPlayer->GetMotion()->IsCurrentMotionEnd(CPlayer::DAMAGE))
		{
			// 待機状態
			m_pMachine->ChangeState<CPlayer_StandState>();
		}
	}

	void OnExit(CPlayer* /*pPlayer*/)override
	{
		m_verticalVelocity = 0.0f;
	}

private:
	float m_verticalVelocity; // 上下方向速度
};

#endif