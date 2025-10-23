//=============================================================================
//
// ブロック処理 [block.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCK_H_
#define _BLOCK_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "objectX.h"
#include "debugproc3D.h"
#include <unordered_map>
#include <functional>

// 前方宣言
class CBlock;

using BlockCreateFunc = std::function<CBlock* ()>;

struct ColliderPart
{
	D3DXVECTOR3 size;
	D3DXVECTOR3 offset;
};

//*****************************************************************************
// ブロッククラス
//*****************************************************************************
class CBlock : public CObjectX
{
public:
	CBlock(int nPriority = 3);
	virtual ~CBlock() = default;

	//*****************************************************************************
	// ブロックの種類
	//*****************************************************************************
	typedef enum
	{
		TYPE_WALL_01 = 0,
		TYPE_FLOOR,
		TYPE_TORCH_01,
		TYPE_ROCK,
		TYPE_GRASS,
		TYPE_WATER,
		TYPE_FLOOR2,

		TYPE_WOODBOX,
		TYPE_SEESAW,
		TYPE_MAX
	}TYPE;

	static CBlock* Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot,D3DXVECTOR3 size, TYPE type);	// ブロックの生成
	virtual btCollisionShape* CreateCollisionShape(const D3DXVECTOR3& size)
	{ 
		return new btBoxShape(btVector3(size.x * 0.5f, size.y * 0.5f, size.z * 0.5f));	// デフォルトはボックス 派生クラスで同じのを作ってShapeを設定
	}
	void CreatePhysics(const D3DXVECTOR3& pos, const D3DXVECTOR3& size);				// コライダーの生成
	void CreatePhysicsFromScale(const D3DXVECTOR3& scale);								// ブロックスケールによるコライダーの生成
	virtual void OnPhysicsCreated(void) {}												// ブロックの特殊処理
	virtual void OnPhysicsReleased(void) {}												// ブロックの特殊処理の破棄
	static void InitFactory(void);
	HRESULT Init(void);
	void Kill(void) { m_isDead = true; }												// ブロック削除
	void Uninit(void);
	void Update(void);
	void UpdateCollider(void);
	void Draw(void);
	void DrawCollider(void);
	void ReleasePhysics(void);															// Physics破棄用
	virtual void Respawn(D3DXVECTOR3 resPos);

	//*****************************************************************************
	// flagment関数
	//*****************************************************************************
	bool IsSelected(void) const { return m_bSelected; }									// ブロックが選択中のフラグを返す
	virtual bool IsDynamicBlock(void) const { return false; }							// 動的ブロックの判別
	virtual bool IsEnd(void) { return false; }
	virtual bool IsGet(void) { return false; }
	virtual bool IsSwitchOn(void) { return false; }

	//*****************************************************************************
	// setter関数
	//*****************************************************************************
	void SetType(TYPE type) { m_Type = type; }											// タイプの設定
	void SetSelected(bool flag) { m_bSelected = flag; }									// 選択中のフラグを返す
	void SetColliderSize(const D3DXVECTOR3& size) { m_colliderSize = size; }			// コライダーサイズの設定
	void SetColliderManual(const D3DXVECTOR3& newSize);									// コライダーサイズの手動設定用
	void SetColliderOffset(const D3DXVECTOR3& offset) { m_colliderOffset = offset; }	// コライダーのオフセットの設定
	void SetEditMode(bool enable);

	//*****************************************************************************
	// getter関数
	//*****************************************************************************
	virtual D3DXCOLOR GetCol(void) const override;										// カラーの取得
	TYPE GetType(void) const { return m_Type; }											// タイプの取得
	btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得
	D3DXVECTOR3 GetColliderSize(void) const { return m_colliderSize; }					// コライダーサイズの取得
	D3DXVECTOR3 GetColliderOffset(void) const { return m_colliderOffset; }				// コライダーのオフセットの取得
	bool IsEditMode(void) const { return m_isEditMode; }								// エディット中かどうか
	virtual btScalar GetMass(void) const { return 2.0f; }								// 質量の取得
	bool IsDead(void) const { return m_isDead; }										// 削除予約の取得
	D3DXMATRIX GetWorldMatrix(void);

	virtual int GetCollisionFlags(void) const { return 0; }// デフォルトはフラグなし
	virtual btVector3 GetLinearFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	virtual btScalar GetRollingFriction(void) const { return 0.7f; }
	virtual btScalar GetFriction(void) const { return 1.0f; }
	virtual D3DXVECTOR3 GetVelocity(void) { return D3DXVECTOR3(0.0f, 0.0f, 0.0f); }
	virtual float CarryTargetDis(void) { return 60.0f; }

private:
	TYPE m_Type;					// 種類
	D3DXCOLOR m_col;				// アルファ値
	D3DXCOLOR m_baseCol;			// ベースのアルファ値
	bool m_bSelected;				// 選択フラグ
	btRigidBody* m_pRigidBody;		// 剛体へのポインタ
	btCollisionShape* m_pShape;		// 当たり判定の形へのポインタ
	CDebugProc3D* m_pDebug3D;		// 3Dデバッグ表示へのポインタ
	D3DXVECTOR3 m_prevSize;			// 前回のサイズ
	D3DXVECTOR3 m_colliderSize;		// コライダーサイズ
	D3DXVECTOR3 m_colliderOffset;	// コライダーの位置
	bool m_isEditMode;				// 編集中かどうか
	static std::unordered_map<TYPE, BlockCreateFunc> m_BlockFactoryMap;
	bool m_isDead;					// 削除予約フラグ
};

#endif