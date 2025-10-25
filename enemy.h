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
#include "guage.h"
#include "charactermanager.h"

//*****************************************************************************
// 敵クラス
//*****************************************************************************
class CEnemy : public CCharacter
{
public:
	CEnemy();
	~CEnemy();

	// 敵モーションの種類
	typedef enum
	{
		NEUTRAL = 0,		// 待機
		MOVE,				// 移動
		ATTACK_01,			// 攻撃1
		MAX
	}ENEMY_MOTION;

	static CEnemy* Create(D3DXVECTOR3 pos, D3DXVECTOR3 rot);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetMove(D3DXVECTOR3 move) { m_move = move; }

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	CMotion* GetMotion(void) { return m_pMotion; }
	D3DXVECTOR3 GetPos(void) { return m_pos; }
	D3DXVECTOR3 GetRot(void) { return m_rot; };
	D3DXVECTOR3 GetSize(void) { return m_size; }
	D3DXVECTOR3 GetMove(void) const { return m_move; }
	D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }
	btScalar GetRadius(void) const { return m_radius; }
	btScalar GetHeight(void) const { return m_height; }
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得

	void ReleasePhysics(void);														// Physics破棄用

private:
	static constexpr int MAX_PARTS = 32;	// 最大パーツ数
	static constexpr float CAPSULE_RADIUS = 14.0f;					// カプセルコライダーの半径
	static constexpr float CAPSULE_HEIGHT = 60.0f;					// カプセルコライダーの高さ
	D3DXVECTOR3 m_pos;						// 位置
	D3DXVECTOR3 m_rot;						// 向き
	D3DXVECTOR3 m_rotDest;					// 向き
	D3DXVECTOR3 m_move;						// 移動量
	D3DXVECTOR3 m_size;						// サイズ
	D3DXVECTOR3 m_colliderPos;				// カプセルコライダーの位置
	D3DXMATRIX m_mtxWorld;					// ワールドマトリックス
	CModel* m_apModel[MAX_PARTS];			// モデル(パーツ)へのポインタ
	CShadowS* m_pShadowS;					// ステンシルシャドウへのポインタ
	CDebugProc3D* m_pDebug3D;				// 3Dデバッグ表示へのポインタ
	CMotion* m_pMotion;						// モーションへのポインタ
	btRigidBody* m_pRigidBody;				// 剛体へのポインタ
	btCollisionShape* m_pShape;				// 当たり判定の形へのポインタ
	btScalar m_radius;						// カプセルコライダーの半径
	btScalar m_height;						// カプセルコライダーの高さ
	int m_nNumModel;						// モデル(パーツ)の総数

	// ステートを管理するクラスのインスタンス
	StateMachine<CEnemy> m_stateMachine;

};


//*****************************************************************************
// 敵の待機状態
//*****************************************************************************
class CEnemy_StandState :public StateBase<CEnemy>
{
public:

	void OnStart(CEnemy* pEnemy)override
	{
		// 待機モーション
		pEnemy->GetMotion()->StartBlendMotion(CEnemy::NEUTRAL, 10);
	}

	void OnUpdate(CEnemy* pEnemy)override
	{
		//// 攻撃状態1へ移行
		//m_pMachine->ChangeState<CEnemy_AttackState1>();

	}

	void OnExit(CEnemy* /*pEnemy*/)override
	{

	}

private:

};

////*****************************************************************************
//// 敵の移動状態
////*****************************************************************************
//class CEnemy_MoveState :public StateBase<CEnemy>
//{
//public:
//
//	void OnStart(CEnemy* pEnemy)override
//	{
//		// 移動モーション
//		pEnemy->GetMotion()->StartBlendMotion(CEnemy::MOVE, 10);
//	}
//
//	void OnUpdate(CEnemy* pEnemy)override
//	{
//
//
//	}
//
//	void OnExit(CEnemy* /*pEnemy*/)override
//	{
//
//	}
//
//private:
//
//};
//
////*****************************************************************************
//// 敵のの攻撃状態1
////*****************************************************************************
//class CEnemy_AttackState1 :public StateBase<CEnemy>
//{
//public:
//
//	void OnStart(CEnemy* pEnemy)override
//	{
//		// 攻撃モーション
//		pEnemy->GetMotion()->StartBlendMotion(CEnemy::ATTACK_01, 10);
//	}
//
//	void OnUpdate(CEnemy* pEnemy)override
//	{
//
//	}
//
//	void OnExit(CEnemy* /*pEnemy*/)override
//	{
//
//	}
//
//private:
//
//};

#endif

