//=============================================================================
//
// 敵の処理 [enemy.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "enemy.h"
#include "guage.h"


// 名前空間stdの使用
using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CEnemy::CEnemy()
{
	// 値のクリア
	memset(m_apModel, 0, sizeof(m_apModel));			// モデル(パーツ)へのポインタ
	m_pos			= INIT_VEC3;						// 位置
	m_rot			= INIT_VEC3;						// 向き
	m_rotDest		= INIT_VEC3;						// 向き
	m_move			= INIT_VEC3;						// 移動量
	m_size			= D3DXVECTOR3(1.0f, 1.0f, 1.0f);	//サイズ
	m_mtxWorld		= {};								// ワールドマトリックス
	m_nNumModel		= 0;								// モデル(パーツ)の総数
	m_pShadowS		= nullptr;							// ステンシルシャドウへのポインタ
	m_pDebug3D		= nullptr;							// 3Dデバッグ表示へのポインタ
	m_pMotion		= nullptr;							// モーションへのポインタ
	m_pRigidBody	= nullptr;							// 剛体へのポインタ
	m_pShape		= nullptr;							// 当たり判定の形へのポインタ
	m_radius		= 0.0f;								// カプセルコライダーの半径
	m_height		= 0.0f;								// カプセルコライダーの高さ
	m_colliderPos	= INIT_VEC3;						// コライダーの位置
}
//=============================================================================
// デストラクタ
//=============================================================================
CEnemy::~CEnemy()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CEnemy* CEnemy::Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CEnemy* pEnemy = new CEnemy;

	pEnemy->m_pos = pos;
	pEnemy->m_rot = D3DXToRadian(rot);

	// 初期化処理
	pEnemy->Init();

	return pEnemy;
}
//=============================================================================
// 初期化処理
//=============================================================================
HRESULT CEnemy::Init(void)
{
	CModel* pModels[MAX_PARTS];
	int nNumModels = 0;

	// パーツの読み込み
	m_pMotion = CMotion::Load("data/motion_enemy.txt", pModels, nNumModels, MAX);

	for (int nCnt = 0; nCnt < nNumModels && nCnt < MAX_PARTS; nCnt++)
	{
		m_apModel[nCnt] = pModels[nCnt];

		// オフセット考慮
		m_apModel[nCnt]->SetOffsetPos(m_apModel[nCnt]->GetPos());
		m_apModel[nCnt]->SetOffsetRot(m_apModel[nCnt]->GetRot());
	}

	// パーツ数を代入
	m_nNumModel = nNumModels;

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
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x", m_pos);
	m_pShadowS->SetStencilRef(2);// 個別のステンシルバッファの参照値を設定

	// インスタンスのポインタを渡す
	m_stateMachine.Start(this);

	// 初期状態のステートをセット
	m_stateMachine.ChangeState<CEnemy_StandState>();

	// HPの設定
	SetHp(100.0f);

	// ゲージを生成
	SetGuages(D3DXVECTOR3(600.0f, 100.0f, 0.0f), 620.0f, 10.0f);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void CEnemy::Uninit(void)
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
void CEnemy::Update(void)
{

#ifdef _DEBUG
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_3))
	{
		// ダメージ処理
		Damage(1.0f);
	}
	else if (pKeyboard->GetTrigger(DIK_4))
	{
		// 回復処理
		Heal(1.0);
	}

#endif

	// ステートマシン更新
	m_stateMachine.Update();

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

	// 現在位置を物理ワールドから取得して m_pos に反映
	btTransform trans;
	m_pRigidBody->getMotionState()->getWorldTransform(trans);
	btVector3 pos = trans.getOrigin();
	m_colliderPos = D3DXVECTOR3(pos.getX(), pos.getY(), pos.getZ());
	m_pos = m_colliderPos - D3DXVECTOR3(0, 45.0f, 0); // 足元へのオフセット

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
void CEnemy::Draw(void)
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
// Physicsの破棄
//=============================================================================
void CEnemy::ReleasePhysics(void)
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
