//=============================================================================
//
// ブロックリスト処理 [blocklist.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "blocklist.h"
#include "blockmanager.h"
#include "player.h"
#include "game.h"
#include "manager.h"
#include "particle.h"
#include <algorithm>
#include "meshcylinder.h"
#include "collisionUtils.h"

// 名前空間stdの使用
using namespace std;

//=============================================================================
// 木箱ブロックのコンストラクタ
//=============================================================================
CWoodBoxBlock::CWoodBoxBlock()
{
	// 値のクリア
	m_ResPos = INIT_VEC3;// リスポーン位置
}
//=============================================================================
// 木箱ブロックのデストラクタ
//=============================================================================
CWoodBoxBlock::~CWoodBoxBlock()
{
	// なし
}
//=============================================================================
// 木箱ブロックの初期化処理
//=============================================================================
HRESULT CWoodBoxBlock::Init(void)
{
	// ブロックの初期化処理
	CBlock::Init();

	// 最初の位置をリスポーン位置に設定
	m_ResPos = GetPos();

	// 動的に戻す
	SetEditMode(false);

	return S_OK;
}
//=============================================================================
// 木箱ブロックの更新処理
//=============================================================================
void CWoodBoxBlock::Update()
{
	CBlock::Update(); // 共通処理
}
//=============================================================================
// リスポーン処理
//=============================================================================
void CWoodBoxBlock::Respawn(D3DXVECTOR3 resPos)
{
	// 動かすためにキネマティックにする
	SetEditMode(true);

	// リスポーン位置に設定
	SetPos(resPos);
	SetRot(D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// コライダーの更新
	UpdateCollider();

	// 動的に戻す
	SetEditMode(false);
}


//=============================================================================
// 灯籠ブロックのコンストラクタ
//=============================================================================
CTorchBlock::CTorchBlock()
{
	// 値のクリア
}
//=============================================================================
// 灯籠ブロックのデストラクタ
//=============================================================================
CTorchBlock::~CTorchBlock()
{
	// なし
}
//=============================================================================
// 灯籠ブロックの更新処理
//=============================================================================
void CTorchBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();
}
//=============================================================================
// 灯籠ブロックのライト更新処理
//=============================================================================
void CTorchBlock::UpdateLight(void)
{
	// 時間の割合を取得
	float progress = CGame::GetTime()->GetProgress(); // 0.0～0.1

	// ======== 各時間帯のメインライト色 ========
	D3DXCOLOR evening(1.0f, 0.65f, 0.35f, 1.0f); // 夕日：濃いオレンジ
	D3DXCOLOR night(0.15f, 0.18f, 0.35f, 1.0f);  // 夜：青みが強く暗い（でも完全には黒くしない）
	D3DXCOLOR morning(0.95f, 0.8f, 0.7f, 1.0f);  // 明け方：柔らかいピンクベージュ

	D3DXCOLOR mainColor;

	// ======== 時間帯ごとに補間 ========
	if (progress < 0.33f)
	{// 夕方
		float t = progress / 0.33f;
		D3DXColorLerp(&mainColor, &evening, &night, t);
	}
	else if (progress < 0.66f)
	{// 夜
		float t = (progress - 0.33f) / 0.33f;
		D3DXColorLerp(&mainColor, &night, &morning, t);
	}
	else
	{// 明け方
		float t = (progress - 0.66f) / 0.34f;
		D3DXColorLerp(&mainColor, &morning, &evening, t); // 少し戻すとループっぽく自然
	}

	// ======== 光の向き補間 ========
	D3DXVECTOR3 dirEvening(0.5f, -1.0f, 0.3f);
	D3DXVECTOR3 dirNight(0.0f, -1.0f, 0.0f);
	D3DXVECTOR3 dirMorning(-0.3f, -1.0f, -0.2f);
	D3DXVECTOR3 mainDir;

	if (progress < 0.5f)
	{
		float t = progress / 0.5f;
		D3DXVec3Lerp(&mainDir, &dirEvening, &dirNight, t);
	}
	else
	{
		float t = (progress - 0.5f) / 0.5f;
		D3DXVec3Lerp(&mainDir, &dirNight, &dirMorning, t);
	}

	D3DXVec3Normalize(&mainDir, &mainDir);

	// 再設定
	CLight::Uninit();

	// ポイントライト
	CLight::AddLight(
		D3DLIGHT_POINT,
		D3DXCOLOR(1.0f, 0.2f, 0.2f, 1.0f) ,
		D3DXVECTOR3(0.0f, -1.0f, 0.0f) ,
		D3DXVECTOR3(GetPos().x, GetPos().y + 20.0f, GetPos().z)
	);
}


//=============================================================================
// 水ブロックのコンストラクタ
//=============================================================================
CWaterBlock::CWaterBlock(int nPriority) : CBlock(nPriority)
{
	// 値のクリア
	m_counter = 0;// 生成カウンター
}
//=============================================================================
// 水ブロックのデストラクタ
//=============================================================================
CWaterBlock::~CWaterBlock()
{
	// なし
}
//=============================================================================
// 水ブロックの更新処理
//=============================================================================
void CWaterBlock::Update(void)
{
	// ブロックの更新処理
	CBlock::Update();

	// プレイヤーの足元に波紋を生成するため、プレイヤーの取得をする
	CPlayer* pPlayer = CGame::GetPlayer();

	// 移動しているときに生成するため、入力を取得する
	InputData input = pPlayer->GatherInput();

	D3DXVECTOR3 pPos = pPlayer->GetColliderPos(); // カプセルコライダー中心位置

	// カプセルとAABBの当たり判定
	bool isOverlap = CCollision::CheckCapsuleAABBCollision(GetPos(), GetModelSize(), GetSize(),
		pPos, pPlayer->GetRadius(), pPlayer->GetHeight());

	if (pPlayer && isOverlap && (input.moveDir.x != 0.0f || input.moveDir.z != 0.0f))
	{
		m_counter++;

		// プレイヤーの位置
		D3DXVECTOR3 pos = pPlayer->GetPos();
		pos.y += 10.0f;

		if (m_counter >= 7)
		{
			// 半径を決めてランダム位置にスポーン
			float radiusMax = 10.0f;

			// 0.0～1.0 の乱数
			float r = (rand() % 10000) / 10000.0f;

			// 平方根を取って均一に分布させる
			float radius = sqrtf(r) * radiusMax;

			float angle = ((rand() % 360) / 180.0f) * D3DX_PI;

			// 位置
			pos.x = pos.x + cosf(angle) * radius;
			pos.y += 2.0f;
			pos.z = pos.z + sinf(angle) * radius;

			// 波紋の生成
			CMeshCylinder::Create(pos, D3DXCOLOR(0.3f, 0.7f, 1.0f, 0.9f), 5.0f, 8.0f, 0.5f, 50, 0.03f);

			m_counter = 0;
		}
	}
	else
	{
		// カウンターをリセット
		m_counter = 0;
	}
}
