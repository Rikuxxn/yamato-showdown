//=============================================================================
//
// ブロックリスト処理 [blocklist.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _BLOCKLIST_H_
#define _BLOCKLIST_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "block.h"

// 前方宣言
class CPlayer;

//*****************************************************************************
// 木箱ブロッククラス
//*****************************************************************************
class CWoodBoxBlock : public CBlock
{
public:
	CWoodBoxBlock();
	~CWoodBoxBlock();

	static TYPE GetStaticType(void) { return TYPE_WOODBOX; }

	HRESULT Init(void);
	void Update(void);
	void Respawn(D3DXVECTOR3 resPos) override;

	bool IsDynamicBlock(void) const override { return true; }
	btVector3 GetAngularFactor(void) const { return btVector3(1.0f, 1.0f, 1.0f); }
	btScalar GetMass(void) const { return 1.0f; }  // 質量の取得

private:
	static constexpr float RESPAWN_HEIGHT = -810.0f;
	D3DXVECTOR3 m_ResPos;	// リスポーン位置
};

//*****************************************************************************
// 灯籠ブロッククラス
//*****************************************************************************
class CTorchBlock : public CBlock
{
public:
	CTorchBlock();
	~CTorchBlock();

	static TYPE GetStaticType(void) { return TYPE_TORCH_01; }

	void Update(void);
	void UpdateLight(void) override;
	float GetDistMax(void) { return m_distMax; }

private:
	static constexpr float m_distMax = 110.0f;

};

//*****************************************************************************
// 水ブロッククラス
//*****************************************************************************
class CWaterBlock : public CBlock
{
public:
	CWaterBlock(int nPriority = 4);
	~CWaterBlock();

	static TYPE GetStaticType(void) { return TYPE_WATER; }

	void Update(void);
	bool IsHit(void) { return m_isHit; }

private:
	int m_counter;	// 生成カウンター
	bool m_isHit;	// 当たっているか
};

//*****************************************************************************
// 埋蔵金ブロッククラス
//*****************************************************************************
class CBuriedTreasureBlock : public CBlock
{
public:
	CBuriedTreasureBlock();
	~CBuriedTreasureBlock();

	static TYPE GetStaticType(void) { return TYPE_BURIED_TREASURE; }

	HRESULT Init(void);
	void Update(void);

	int GetCollisionFlags(void) const override { return btCollisionObject::CF_NO_CONTACT_RESPONSE; }

private:
	static constexpr float TRIGGER_DISTANCE = 100.0f;	// 判定距離
	static constexpr int SPAWN_TIME = 180;				// 生成までの時間

	int m_effectTimer;									// エフェクト生成タイマー
};

//*****************************************************************************
// 扉ブロッククラス
//*****************************************************************************
class CDoorBlock : public CBlock
{
public:
	CDoorBlock();
	~CDoorBlock();

	static TYPE GetStaticType(void) { return TYPE_DOOR; }

	int GetCollisionFlags(void) const override { return btCollisionObject::CF_NO_CONTACT_RESPONSE; }

	void Update(void);

private:

};

//*****************************************************************************
// 出口判定ブロッククラス
//*****************************************************************************
class CExitBlock : public CBlock
{
public:
	CExitBlock();
	~CExitBlock();

	static TYPE GetStaticType(void) { return TYPE_GHOSTOBJECT; }

	void Update(void);
	bool IsHitPlayer(CPlayer* pPlayer);

	bool IsEscape(void) { return m_isEscape; }
private:
	bool m_isEscape;	// 脱出したかどうか
};

#endif
