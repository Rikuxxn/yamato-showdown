//=============================================================================
//
// �p�[�e�B�N������ [particle.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _PARTICLE_H_// ���̃}�N����`������Ă��Ȃ�������
#define _PARTICLE_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "main.h"
#include "effect.h"
#include <unordered_map>
#include <functional>

// �O���錾
class CParticle;

//*****************************************************************************
// �p�[�e�B�N���N���X
//*****************************************************************************
class CParticle : public CEffect
{
public:
	CParticle();
	~CParticle();

	//*************************************************************************
	// �p�[�e�B�N�������e���v���[�g
	//*************************************************************************
	// particleType : �����������p�[�e�B�N���N���X��
	template <typename particleType>
	static particleType* Create(D3DXVECTOR3 dir, D3DXVECTOR3 pos, D3DXCOLOR col, int nLife, int nMaxParticle)
	{
		// �n���ꂽ�^��CParticle���p�����Ă��Ȃ������ꍇ
		static_assert(std::is_base_of<CParticle, particleType>::value, "�^��CParticle���p�����Ă��܂���B");

		particleType* pParticle = new particleType();// �^�̃C���X�^���X����

		if (!pParticle)
		{
			return nullptr;
		}

		// ����������
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
	D3DXVECTOR3 m_Dir;	// ����
	int m_nLife;		// ����
	int m_nMaxParticle;	// ���q�̐�
};

//*****************************************************************************
// ���p�[�e�B�N���N���X
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
// �Α��̉��p�[�e�B�N���N���X
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
// �Ή����˃p�[�e�B�N���N���X
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
	float m_speedScale;  // ���x�{��
};

//*****************************************************************************
// �����Ԃ��p�[�e�B�N���N���X
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
// �I�[���p�[�e�B�N���N���X
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
// �I�[��(���ʗp)�p�[�e�B�N���N���X
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
// �ړ����p�[�e�B�N���N���X
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
// ���V�p�[�e�B�N���N���X
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
// ����(�r��)�p�[�e�B�N���N���X
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
// �o�p�[�e�B�N���N���X
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