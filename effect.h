//=============================================================================
//
// �G�t�F�N�g���� [effect.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _EFFECT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _EFFECT_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "objectbillboard.h"
#include "block.h"

// �G�t�F�N�g�\����
struct EffectDesc
{
	const char*	  path			= nullptr;
	D3DXVECTOR3   pos			= INIT_VEC3;
	D3DXVECTOR3   move			= INIT_VEC3;
	D3DXCOLOR     col			= INIT_XCOL;
	float         fRadius		= 0.0f;
	int           nLife			= 0;
	float         fGravity		= 0.0f;
	float         fDecRadius	= 0.0f;
};

//*****************************************************************************
// �G�t�F�N�g�N���X
//*****************************************************************************
class CEffect : public CObjectBillboard
{
public:
	CEffect(int nPriority = 5);
	~CEffect();

	static CEffect* Create(const EffectDesc& desc);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetMove(D3DXVECTOR3 move) { m_move = move; }
	void SetLife(int nLife) { m_nLife = nLife; }
	void SetRadius(float fRadius) { m_fRadius = fRadius; }
	void SetGravity(float fGrav) { m_fGravity = fGrav; }
	void SetDecRadius(float fDecRad) { m_fDecRadius = fDecRad; }
	float GetRadius(void) { return m_fRadius; }
	float GetGravity(void) { return m_fGravity; }

private:
	D3DXVECTOR3 m_move;		// �ړ���
	float m_fRadius;		// ���a
	int m_nLife;			// ����
	int m_nIdxTexture;		// �e�N�X�`���C���f�b�N�X
	float m_fGravity;		// �d��
	float m_fDecRadius;		// ���a�̌�����
};

//*****************************************************************************
// ���f���G�t�F�N�g�N���X
//*****************************************************************************
class CModelEffect : public CObjectX
{
public:
	CModelEffect();
	~CModelEffect();

	static CModelEffect* Create(const char* path, D3DXVECTOR3 pos, D3DXVECTOR3 rot, D3DXVECTOR3 velocity, D3DXVECTOR3 size, 
		int nLife, float fGravity, float scaleDec);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);

	void SetVelocity(D3DXVECTOR3 velocity) { m_velocity = velocity; }
	void SetGravity(float fGrav) { m_fGravity = fGrav; }
	void SetScaleDec(float fDec) { m_scaleDec = fDec; }
	void SetLife(int nLife) { m_nLife = nLife; }

private:
	int m_nLife;			// ����
	float m_fGravity;		// �d�͉����x
	float m_scaleDec;		// �T�C�Y������
	D3DXVECTOR3 m_velocity; // ���x
};

#endif