//=============================================================================
//
// パーティクル処理 [particle.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_H_// このマクロ定義がされていなかったら
#define _PARTICLE_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "main.h"
#include "effect.h"
#include <unordered_map>
#include <functional>

// 前方宣言
class CParticle;

//*****************************************************************************
// パーティクルクラス
//*****************************************************************************
class CParticle : public CEffect
{
public:
	CParticle();
	~CParticle();

	//*************************************************************************
	// パーティクル生成テンプレート
	//*************************************************************************
	// particleType : 生成したいパーティクルクラス名
	template <typename particleType>
	static particleType* Create(D3DXVECTOR3 dir, D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nMaxParticle)
	{
		// 渡された型がCParticleを継承していなかった場合
		static_assert(std::is_base_of<CParticle, particleType>::value, "型はCParticleを継承していません。");

		particleType* pParticle = new particleType();// 型のインスタンス生成

		if (!pParticle)
		{
			return nullptr;
		}

		// 初期化処理
		pParticle->Init();

		if (dir != INIT_VEC3)
		{
			pParticle->SetDir(dir);
		}

		pParticle->SetPos(pos);
		pParticle->SetCol(col);
		pParticle->SetLife(nLife);
		pParticle->m_nMaxParticle = nMaxParticle;

		return pParticle;
	}

	HRESULT Init(void);
	void Update(void);

	void SetLife(int nLife) { m_nLife = nLife; }
	void SetDir(D3DXVECTOR3 dir) { m_Dir = dir; }

	int GetLife(void) { return m_nLife; }
	int GetMaxParticle(void) { return m_nMaxParticle; }
	D3DXVECTOR3 GetDir(void) { return m_Dir; }

private:
	D3DXVECTOR3 m_Dir;	// 方向
	int m_nLife;		// 寿命
	int m_nMaxParticle;	// 粒子の数
};

//*****************************************************************************
// 炎パーティクルクラス
//*****************************************************************************
class CFireParticle : public CParticle
{
public:
	CFireParticle();
	~CFireParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 石像の炎パーティクルクラス
//*****************************************************************************
class CStatueFireParticle : public CParticle
{
public:
	CStatueFireParticle();
	~CStatueFireParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 火炎放射パーティクルクラス
//*****************************************************************************
class CFlamethrowerParticle : public CParticle
{
public:
	CFlamethrowerParticle();
	~CFlamethrowerParticle();

	HRESULT Init(void);
	void Update(void);

	void SetSpeedScale(float scale) { m_speedScale = scale; }
	float GetSpeedScale() const { return m_speedScale; }

private:
	float m_speedScale;  // 速度倍率
};

//*****************************************************************************
// 水しぶきパーティクルクラス
//*****************************************************************************
class CWaterParticle : public CParticle
{
public:
	CWaterParticle();
	~CWaterParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// オーラパーティクルクラス
//*****************************************************************************
class CAuraParticle : public CParticle
{
public:
	CAuraParticle();
	~CAuraParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// オーラ(仮面用)パーティクルクラス
//*****************************************************************************
class CAura2Particle : public CParticle
{
public:
	CAura2Particle();
	~CAura2Particle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 移動時パーティクルクラス
//*****************************************************************************
class CMoveParticle : public CParticle
{
public:
	CMoveParticle();
	~CMoveParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 浮遊パーティクルクラス
//*****************************************************************************
class CFloatingParticle : public CParticle
{
public:
	CFloatingParticle();
	~CFloatingParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 水流(排水)パーティクルクラス
//*****************************************************************************
class CWaterFlowParticle : public CParticle
{
public:
	CWaterFlowParticle();
	~CWaterFlowParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

//*****************************************************************************
// 塵パーティクルクラス
//*****************************************************************************
class CDustParticle : public CParticle
{
public:
	CDustParticle();
	~CDustParticle();

	HRESULT Init(void);
	void Update(void);

private:

};

#endif