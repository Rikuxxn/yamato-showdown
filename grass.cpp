//=============================================================================
//
// 草処理 [grass.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "grass.h"
#include "manager.h"
#include "game.h"
#include "easing.h"
#include "player.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CGrassBlock::CGrassBlock(int nPriority) : CBlock(nPriority)
{
	// 値のクリア
	m_rotVel = INIT_VEC3;
}
//=============================================================================
// デストラクタ
//=============================================================================
CGrassBlock::~CGrassBlock()
{
	// なし
}
//=============================================================================
// 更新処理
//=============================================================================
void CGrassBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	if (!CGame::GetPlayer())
	{
		return;
	}

	// プレイヤーの位置を取得
	D3DXVECTOR3 playerPos = CGame::GetPlayer()->GetPos();

	// 草とプレイヤーの距離
	D3DXVECTOR3 diff = playerPos - GetPos();
	float dist = D3DXVec3Length(&diff);

	D3DXVECTOR3 rot = GetRot();
	float distMax = 60.0f;				// この距離以内なら傾く
	float fMaxTilt = D3DXToRadian(60);  // 最大の傾き角度

	// 目標の回転角度
	D3DXVECTOR3 targetRot = D3DXVECTOR3(0, 0, 0);

	if (dist < distMax) // 範囲内
	{
		// 正規化
		D3DXVec3Normalize(&diff, &diff);
		
		// 距離に応じた割合
		float t = 1.0f - (dist / distMax);

		// 傾き角度 = 最大角度 × 割合
		float tilt = fMaxTilt * t;

		rot.x = -diff.z * tilt;
		rot.z = diff.x * tilt;
	}

	// *** バネ ＋ ダンピング ***
	float stiffness = 0.12f;		// バネの強さ（0.1～0.3ぐらい）
	float damping = 0.75f;			// 減衰率（0.8～0.98ぐらい）

	// バネ力 = (目標 - 現在) × バネ定数
	m_rotVel.x += (targetRot.x - rot.x) * stiffness;
	m_rotVel.z += (targetRot.z - rot.z) * stiffness;

	// 減衰（抵抗）
	m_rotVel.x *= damping;
	m_rotVel.z *= damping;

	// 回転に反映
	rot.x += m_rotVel.x;
	rot.z += m_rotVel.z;

	// 向きの設定
	SetRot(rot);
}
//=============================================================================
// 描画処理
//=============================================================================
void CGrassBlock::Draw(void)
{
	// デバイスの取得
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// αブレンディングを加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	// αテストを有効
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//デフォルトはfalse
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0より大きかったら描画

	// ブロックの描画
	CBlock::Draw();

	// αテストを無効に戻す
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//デフォルトはfalse

	// αブレンディングを元に戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
