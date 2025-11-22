//=============================================================================
//
// 武器の当たり判定処理 [weaponcollider.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "weaponcollider.h"
#include "enemy.h"
#include "player.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CWeaponCollider::CWeaponCollider()
{
	// 値のクリア
	m_prevBase = INIT_VEC3;
	m_prevTip = INIT_VEC3;
	m_currBase = INIT_VEC3;
	m_currTip = INIT_VEC3;
	m_bActive = false;
	m_bHasHit = false;
}
//=============================================================================
// デストラクタ
//=============================================================================
CWeaponCollider::~CWeaponCollider()
{
	// なし
}
//=============================================================================
// 当たり判定処理(敵)
//=============================================================================
void CWeaponCollider::CheckHit(CEnemy* pEnemy, float fDamage)
{
    if (!m_bActive || m_bHasHit || !pEnemy)
    {
        return;
    }

    // カプセルの上下端点
    D3DXVECTOR3 top = pEnemy->GetPos() + D3DXVECTOR3(0, pEnemy->GetHeight() * 0.5f, 0);
    D3DXVECTOR3 bottom = pEnemy->GetPos() - D3DXVECTOR3(0, pEnemy->GetHeight() * 0.5f, 0);
    float radius = pEnemy->GetRadius();

    float weaponRadius = 35.0f; // 当たり判定の半径

    if (CCollision::IntersectSegmentCapsule(m_prevBase, m_currBase, bottom, top, radius + weaponRadius) ||
        CCollision::IntersectSegmentCapsule(m_prevTip, m_currTip, bottom, top, radius + weaponRadius) ||
        CCollision::IntersectSegmentCapsule(m_prevBase, m_currTip, bottom, top, radius + weaponRadius))
    {
        // ダメージ処理
        pEnemy->Damage(fDamage);
        m_bHasHit = true; // 当たった
    }

    // 座標更新
    m_prevBase = m_currBase;
    m_prevTip = m_currTip;
}
//=============================================================================
// 当たり判定処理(プレイヤー)
//=============================================================================
void CWeaponCollider::CheckHit(CPlayer* pPlayer, float fDamage)
{
    if (!m_bActive || m_bHasHit || !pPlayer)
    {
        return;
    }

    // カプセルの上下端点
    D3DXVECTOR3 top = pPlayer->GetPos() + D3DXVECTOR3(0, pPlayer->GetHeight() * 0.5f, 0);
    D3DXVECTOR3 bottom = pPlayer->GetPos() - D3DXVECTOR3(0, pPlayer->GetHeight() * 0.5f, 0);
    float radius = pPlayer->GetRadius();

    float weaponRadius = 45.0f; // 当たり判定の半径

    if (CCollision::IntersectSegmentCapsule(m_prevBase, m_currBase, bottom, top, radius + weaponRadius) ||
        CCollision::IntersectSegmentCapsule(m_prevTip, m_currTip, bottom, top, radius + weaponRadius) ||
        CCollision::IntersectSegmentCapsule(m_prevBase, m_currTip, bottom, top, radius + weaponRadius))
    {
        // ダメージ処理
        pPlayer->Damage(fDamage);
        m_bHasHit = true; // 当たった
    }

    // 座標更新
    m_prevBase = m_currBase;
    m_prevTip = m_currTip;
}
