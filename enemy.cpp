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
#include "enemyState.h"
#include "ui.h"

// 名前空間stdの使用
using namespace std;

//=============================================================================
// コンストラクタ
//=============================================================================
CEnemy::CEnemy()
{
	// 値のクリア
	memset(m_apModel, 0, sizeof(m_apModel));			// モデル(パーツ)へのポインタ
	m_mtxWorld			= {};							// ワールドマトリックス
	m_nNumModel			= 0;							// モデル(パーツ)の総数
	m_pShadowS			= nullptr;						// ステンシルシャドウへのポインタ
	m_pDebug3D			= nullptr;						// 3Dデバッグ表示へのポインタ
	m_pMotion			= nullptr;						// モーションへのポインタ
	m_pTipModel			= nullptr;						// 武器コライダー用モデル
	m_pBaseModel		= nullptr;						// 武器コライダー用モデル
	m_pWeaponCollider	= nullptr;						// 武器の当たり判定へのポインタ
	m_requestedAction = ACTION_NONE;
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

	pEnemy->SetPos(pos);
	pEnemy->SetRot(D3DXToRadian(rot));

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

		// 名前に weapon が含まれていたら武器パーツと認識
		if (strstr(m_apModel[nCnt]->GetPath(), "weapon") != nullptr)
		{
			m_pSwordModel = m_apModel[nCnt];
		}
	}

	// パーツ数を代入
	m_nNumModel = nNumModels;

	// 武器コライダーの生成
	m_pWeaponCollider = make_unique<CWeaponCollider>();

#ifdef _DEBUG
	// 武器コライダーモデルの生成
	m_pTipModel = CObjectX::Create("data/MODELS/weapon_collider.x", m_pWeaponCollider->GetCurrentTipPos(), INIT_VEC3, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
	m_pBaseModel = CObjectX::Create("data/MODELS/weapon_collider.x", m_pWeaponCollider->GetCurrentBasePos(), INIT_VEC3, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
#endif

	// 最初の向き
	SetRot(D3DXVECTOR3(0.0f, -D3DX_PI, 0.0f));

	// カプセルコライダーの設定
	CreatePhysics(CAPSULE_RADIUS, CAPSULE_HEIGHT, 5.0f);

	// ステンシルシャドウの生成
	m_pShadowS = CShadowS::Create("data/MODELS/stencilshadow.x", GetPos());
	m_pShadowS->SetStencilRef(2);// 個別のステンシルバッファの参照値を設定

	// インスタンスのポインタを渡す
	m_stateMachine.Start(this);

	// 初期状態のステートをセット
	m_stateMachine.ChangeState<CEnemy_StandState>();

	// 敵AIの生成
	m_pAI = make_unique<CEnemyAI>();

	// HPの設定
	SetHp(100.0f);

	// ゲージを生成
	SetGuages(D3DXVECTOR3(570.0f, 100.0f, 0.0f), 620.0f, 10.0f);

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
	// 武器コライダーの更新
	m_pWeaponCollider->Update(m_pSwordModel, 50.0f, 10.0f);

#ifdef _DEBUG
	CInputKeyboard* pKeyboard = CManager::GetInputKeyboard();

	if (pKeyboard->GetTrigger(DIK_3))
	{
		// ダメージ処理
		Damage(50.0f);
	}
	else if (pKeyboard->GetTrigger(DIK_4))
	{
		// 回復処理
		Heal(1.0);
	}

	// 武器コライダー用モデルの位置更新
	m_pTipModel->SetPos(m_pWeaponCollider->GetCurrentTipPos());
	m_pBaseModel->SetPos(m_pWeaponCollider->GetCurrentBasePos());

#endif

	// AIを更新（現在の行動のリクエスト）
	if (m_pAI)
	{
		m_pAI->Update(this, CGame::GetPlayer());
	}

	// ステートマシン更新
	m_stateMachine.Update();

	// 向きの更新処理
	UpdateRotation(0.09f);

	// コライダーの位置更新(オフセットを設定)
	UpdateCollider(D3DXVECTOR3(0, 45.0f, 0));// 足元に合わせる

	if (m_pShadowS != nullptr)
	{
		// ステンシルシャドウの位置設定
		m_pShadowS->SetPosition(GetPos());
	}

	// モーションの更新処理
	m_pMotion->Update(m_apModel, m_nNumModel);
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
	D3DXMatrixScaling(&mtxSize, GetSize().x, GetSize().y, GetSize().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxSize);

	// 向きを反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, GetRot().y, GetRot().x, GetRot().z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// 位置を反映
	D3DXMatrixTranslation(&mtxTrans, GetPos().x, GetPos().y, GetPos().z);
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

	//btRigidBody* pRigid = GetRigidBody();
	//btCollisionShape* pShape = GetCollisionShape();

	//// カプセルコライダーの描画
	//if (pRigid && pShape)
	//{
	//	btTransform transform;
	//	pRigid->getMotionState()->getWorldTransform(transform);

	//	m_pDebug3D->DrawCapsuleCollider((btCapsuleShape*)pShape, transform, D3DXCOLOR(1, 1, 1, 1));
	//}

#endif

}
//=============================================================================
// 前方ベクトル取得
//=============================================================================
D3DXVECTOR3 CEnemy::GetForward(void)
{
	// プレイヤーの回転角度（Y軸）から前方ベクトルを計算
	float yaw = GetRot().y;

	D3DXVECTOR3 forward(-sinf(yaw), 0.0f, -cosf(yaw));

	// 正規化する
	D3DXVec3Normalize(&forward, &forward);

	return forward;
}
//=============================================================================
// ダメージ処理
//=============================================================================
void CEnemy::Damage(float fDamage)
{
	if (!m_pMotion->IsCurrentMotion(DAMAGE) && !m_pMotion->IsCurrentMotion(GUARD))
	{
		// まず共通のHP処理
		CCharacter::Damage(fDamage);

		if (!m_pMotion->IsCurrentMotion(ACCUMULATION) && !m_pMotion->IsCurrentMotion(GUARD) &&
			m_pMotion->IsCurrentMotion(CLOSE_ATTACK_01))
		{
			// ダメージステートへ
			m_stateMachine.ChangeState<CEnemy_DamageState>();
		}
	}

	// 死亡時
	if (IsDead())
	{
		// 死亡状態
		m_stateMachine.ChangeState<CEnemy_DeadState>();
		return;
	}
}