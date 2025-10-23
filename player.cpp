//=============================================================================
//
// プレイヤー処理 [player.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "player.h"
#include "texture.h"
#include "model.h"
#include "particle.h"
#include "game.h"

// 名前空間stdの使用
using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CPlayer::CPlayer(int nPriority) : CObject(nPriority)
{
	// 値のクリア
	memset(m_apModel, 0, sizeof(m_apModel));			// モデル(パーツ)へのポインタ
	m_pos				= INIT_VEC3;					// 位置
	m_rot				= INIT_VEC3;					// 向き
	m_rotDest			= INIT_VEC3;					// 向き
	m_move				= INIT_VEC3;					// 移動量
	m_targetMove		= INIT_VEC3;					// 目標速度
	m_currentMove		= INIT_VEC3;					// 実際の移動速度
	m_size				= D3DXVECTOR3(1.0f, 1.0f, 1.0f);// サイズ
	m_mtxWorld			= {};							// ワールドマトリックス
	m_nNumModel			= 0;							// モデル(パーツ)の総数
	m_playerUse			= true;							// 使われているかどうか
	m_pShadowS			= nullptr;						// ステンシルシャドウへのポインタ
	m_pMotion			= nullptr;						// モーションへのポインタ
	m_bIsMoving			= false;						// 移動入力フラグ
	m_pRigidBody		= nullptr;						// 剛体へのポインタ
	m_bOnGround			= false;						// 接地フラグ
	m_pShape			= nullptr;						// 当たり判定の形へのポインタ
	m_pDebug3D			= nullptr;						// 3Dデバッグ表示へのポインタ
	m_radius			= 0.0f;							// カプセルコライダーの半径
	m_height			= 0.0f;							// カプセルコライダーの高さ
	m_colliderPos		= INIT_VEC3;					// コライダーの位置
	m_pCarryingBlock	= nullptr;						// 運んでいるブロック
	m_particleTimer		= 0;							// パーティクルタイマー

}
//=============================================================================
// デストラクタ
//=============================================================================
CPlayer::~CPlayer()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CPlayer* CPlayer::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CPlayer* pPlayer = new CPlayer;

	pPlayer->m_pos = pos;
	pPlayer->m_rot = D3DXToRadian(rot);

	// 初期化処理
	pPlayer->Init();

	return pPlayer;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CPlayer::Init(void)
{
	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// パーツの読み込み
	m_pMotion = CMotion::Load("data/motion.txt", pModels, nNumModels, MAX);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// オフセット考慮
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// パーツ数を代入
	m_nNumModel = nNumModels;

	// プレイヤーが使われている
	m_playerUse = true;

	// 変数の初期化
	m_rot = D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f);

	//*********************************************************************
	// Bullet Physics カプセルコライダーの設定
	//*********************************************************************

	m_radius = CAPSULE_RADIUS;
	m_height = CAPSULE_HEIGHT;

	m_pShape = new btCapsuleShape(m_radius, m_height);

	// コライダー中心 = 足元 + オフセット
	m_colliderPos = m_pos + D3DXVECTOR3(0, 20.0f, 0);

	btTransform transform;
	transform.setIdentity();
	transform.setOrigin(btVector3(m_colliderPos.x, m_colliderPos.y, m_colliderPos.z));

	// 質量を設定
	btScalar mass = 2.0f;
	btVector3 inertia(0, 0, 0);  // 慣性

	m_pShape->calculateLocalInertia(mass, inertia);

	btDefaultMotionState* motionState = new btDefaultMotionState(transform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, motionState, m_pShape, inertia);

	m_pRigidBody = new btRigidBody(rbInfo);

	m_pRigidBody->setAngularFactor(btVector3(0, 0, 0));
	m_pRigidBody->setFriction(0.0f);// 摩擦
	m_pRigidBody->setRollingFriction(0.0f);// 転がり摩擦

	m_pRigidBody->setUserPointer(this);
	m_pRigidBody->setActivationState(DISABLE_DEACTIVATION);// スリープ状態にしない

	// 物理ワールドに追加
	btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();

	if (pWorld != nullptr)
	{
		pWorld->addRigidBody(m_pRigidBody);
	}

	// ステンシルシャドウの生成
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x",m_pos);

	// インスタンスのポインタを渡す
	m_stateMachine.Start(this);

	// 初期状態のステートをセット
	m_stateMachine.ChangeState<CPlayer_StandState>();

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CPlayer::Uninit(void)
{
	ReleasePhysics();

	for (int nCnt = 0; nCnt < MAX_PARTS; nCnt++)
	{
		if (m_apModel[nCnt] != nullptr)
		{
			m_apModel[nCnt]->Uninit();
			delete m_apModel[nCnt];
			m_apModel[nCnt] = nullptr;
		}
	}

	if (m_pMotion != nullptr)
	{
		delete m_pMotion;
		m_pMotion = nullptr;
	}

	// オブジェクトの破棄(自分自身)
	this->Release();
}
//=============================================================================
// 更新処理
//=============================================================================
void CPlayer::Update(void)
{
	// カメラの取得
	CCamera* pCamera = CManager::GetCamera();

	// カメラの角度の取得
	D3DXVECTOR3 CamRot = pCamera->GetRot();

	// 接地判定
	m_bOnGround = OnGround(CManager::GetPhysicsWorld(), m_pRigidBody, 55.0f);

	// ステートマシン更新
	m_stateMachine.Update();

	// 入力判定の取得
	InputData input = GatherInput();

	// 向きの正規化
	if (m_rotDest.y - m_rot.y > D3DX_PI)
	{
		m_rot.y += D3DX_PI * 2.0f;
	}
	else if (m_rotDest.y - m_rot.y < -D3DX_PI)
	{
		m_rot.y -= D3DX_PI * 2.0f;
	}

	m_rot.y += (m_rotDest.y - m_rot.y) * 0.09f;

	// 移動入力があればプレイヤー向きを入力方向に
	if (!m_pMotion->IsAttacking() && (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f))
	{
		m_rotDest.y = atan2f(-input.moveDir.x,-input.moveDir.z);
	}

	// 現在位置を物理ワールドから取得して m_pos に反映
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 35.0f, 0); // 足元へのオフセット

	CModelEffect* pModelEffect = nullptr;

	if (m_bIsMoving && m_bOnGround)
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
			pModelEffect = CModelEffect::Create("data/MODELS/effectModel_step.x", m_pos, rot,
				move, D3DXVECTOR3(0.3f, 0.3f, 0.3f), 180, 0.01f, 0.008f);
		}
	}
	else
	{
		m_particleTimer = 0; // 停止時はリセット
	}

	if (m_pShadowS != nullptr)
	{
		// ステンシルシャドウの位置設定
		m_pShadowS->SetPosition(m_pos);
	}

	int nNumModels = 16;

	// モーションの更新処理
	m_pMotion->Update(m_apModel, nNumModels);
}
//=============================================================================
// 描画処理
//=============================================================================
void CPlayer::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// 計算用マトリックス
	D3DXMATRIX mtxRot, mtxTrans, mtxSize;

	// ワールドマトリックスの初期化
	D3DXMatrixIdentity(&m_mtxWorld);

	// サイズを反映
	D3DXMatrixScaling(&mtxSize, m_size.x, m_size.y, m_size.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ワールドマトリックスを設定
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	for (int nCntMat = 0; nCntMat < m_nNumModel; nCntMat++)
	{
		// モデル(パーツ)の描画
		if (m_apModel[nCntMat])
		{
			m_apModel[nCntMat]->Draw();
		}
	}

#ifdef _DEBUG

	// カプセルコライダーの描画
	if (m_pRigidBody && m_pShape)
	{
		btTransform transform;
		m_pRigidBody->getMotionState()->getWorldTransform(transform);

		m_pDebug3D->DrawCapsuleCollider((btCapsuleShape*)m_pShape, transform, D3DXCOLOR(1, 1, 1, 1));
	}

#endif

}
//=============================================================================
// レイによる接触判定
//=============================================================================
bool CPlayer::OnGround(btDiscreteDynamicsWorld* world, btRigidBody* playerBody, float rayLength)
{
	btTransform trans;
	playerBody->getMotionState()->getWorldTransform(trans);

	btVector3 start = trans.getOrigin();
	btVector3 end = start - btVector3(0, rayLength, 0);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}
	};

	RayResultCallback rayCallback(start, end);
	world->rayTest(start, end, rayCallback);

	if (rayCallback.hasHit())
	{// 接触
		// ヒットした面の法線
		btVector3 normal = rayCallback.m_hitNormalWorld.normalized();

		// 上方向ベクトル
		btVector3 up(0, 1, 0);

		// 法線と上方向の内積（= cosθ）
		float slopeDot = normal.dot(up);

		// cosθ が小さいほど急斜面
		// 50度までを地面として扱う
		float slopeLimit = cosf(D3DXToRadian(50.0f));

		if (slopeDot >= slopeLimit)
		{
			return true; // 接地
		}
		else
		{
			return false; // 急斜面なので接地扱いしない
		}
	}

	return false;
}
//=============================================================================
// Physicsの破棄
//=============================================================================
void CPlayer::ReleasePhysics(void)
{
	if (m_pRigidBody)
	{
		btDiscreteDynamicsWorld* pWorld = CManager::GetPhysicsWorld();
		if (pWorld) pWorld->removeRigidBody(m_pRigidBody);
		delete m_pRigidBody->getMotionState();
		delete m_pRigidBody;
		m_pRigidBody = nullptr;
	}

	if (m_pShape)
	{
		delete m_pShape;
		m_pShape = nullptr;
	}
}
//=============================================================================
// プレイヤーの前方ベクトル取得
//=============================================================================
D3DXVECTOR3 CPlayer::GetForward(void) const
{
	// プレイヤーの回転角度（Y軸）から前方ベクトルを計算
	float yaw = m_rot.y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// 正規化する
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// プレイヤーの前方レイ判定処理
//=============================================================================
CBlock* CPlayer::FindFrontBlockByRaycast(float rayLength)
{
	btDiscreteDynamicsWorld* world = CManager::GetPhysicsWorld();

	if (!world)
	{
		return nullptr;
	}

	D3DXVECTOR3 from = GetPos() + D3DXVECTOR3(0.0f,20.0f,0.0f);
	D3DXVECTOR3 to = from + GetForward() * rayLength;

	btVector3 btFrom(from.x, from.y, from.z);
	btVector3 btTo(to.x, to.y, to.z);

	struct RayResultCallback : public btCollisionWorld::ClosestRayResultCallback
	{
		RayResultCallback(const btVector3& from, const btVector3& to)
			: btCollisionWorld::ClosestRayResultCallback(from, to) {}

		virtual btScalar addSingleResult(btCollisionWorld::LocalRayResult& rayResult, bool normalInWorldSpace)
		{
			// ブロック以外を無視する場合はここでフィルターしてもいい
			return ClosestRayResultCallback::addSingleResult(rayResult, normalInWorldSpace);
		}
	};

	RayResultCallback rayCallback(btFrom, btTo);
	world->rayTest(btFrom, btTo, rayCallback);

	if (rayCallback.hasHit())
	{
		void* userPtr = rayCallback.m_collisionObject->getUserPointer();

		if (userPtr)
		{
			CBlock* pBlock = static_cast<CBlock*>(userPtr);

			if (pBlock->IsDynamicBlock())
			{
				return pBlock; // 動的なブロックが前方にあった
			}
		}
	}

	return nullptr;
}
//=============================================================================
// 入力判定取得関数
//=============================================================================
InputData CPlayer::GatherInput(void)
{
	InputData input{};
	input.moveDir = D3DXVECTOR3(0, 0, 0);
	input.attack = false;

	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();	// キーボードの取得
	CInputMouse* pMouse = CManager::GetInputMouse();			// マウスの取得
	CInputJoypad* pJoypad = CManager::GetInputJoypad();			// ジョイパッドの取得
	XINPUT_STATE* pStick = CInputJoypad::GetStickAngle();		// スティックの取得
	CCamera* pCamera = CManager::GetCamera();					// カメラの取得
	D3DXVECTOR3 CamRot = pCamera->GetRot();						// カメラ角度の取得

	// ---------------------------
	// 攻撃入力
	// ---------------------------
	if (pMouse->GetTrigger(0) || pJoypad->GetTrigger(pJoypad->JOYKEY_X))
	{
		input.attack = true;
	}

	// ---------------------------
	// 攻撃中は移動入力無効化
	// ---------------------------
	if (m_pMotion->IsAttacking())
	{
		return input;
	}

	// ---------------------------
	// ゲームパッド入力
	// ---------------------------
	if (pJoypad->GetStick() && pStick)
	{
		float stickX = pStick->Gamepad.sThumbLX;
		float stickY = pStick->Gamepad.sThumbLY;
		float magnitude = sqrtf(stickX * stickX + stickY * stickY);
		const float DEADZONE = 10922.0f;

		if (magnitude >= DEADZONE)
		{
			stickX /= magnitude;
			stickY /= magnitude;
			float normMag = std::min((magnitude - DEADZONE) / (32767.0f - DEADZONE), 1.0f);
			stickX *= normMag;
			stickY *= normMag;

			D3DXVECTOR3 dir;
			float yaw = CamRot.y;

			dir.x = -(stickX * cosf(yaw) + stickY * sinf(yaw));
			dir.z = stickX * sinf(-yaw) + stickY * cosf(yaw);
			dir.z = -dir.z;

			input.moveDir += D3DXVECTOR3(dir.x, 0, dir.z);
		}
	}

	// ---------------------------
	// キーボード入力
	// ---------------------------
	if (pKeyboard->GetPress(DIK_W))
	{
		input.moveDir += D3DXVECTOR3(-sinf(CamRot.y), 0, -cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_S))
	{
		input.moveDir += D3DXVECTOR3(sinf(CamRot.y), 0, cosf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_A))
	{
		input.moveDir += D3DXVECTOR3(cosf(CamRot.y), 0, -sinf(CamRot.y));
	}
	if (pKeyboard->GetPress(DIK_D))
	{
		input.moveDir += D3DXVECTOR3(-cosf(CamRot.y), 0, sinf(CamRot.y));
	}

	// 正規化
	if (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f)
	{
		D3DXVec3Normalize(&input.moveDir, &input.moveDir);
	}

	return input;
}

