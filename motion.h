//=============================================================================
//
// ���[�V�������� [motion.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MOTION_H_// ���̃}�N����`������Ă��Ȃ�������
#define _MOTION_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "model.h"

//*****************************************************************************
// ���[�V�����N���X
//*****************************************************************************
class CMotion
{
public:
	CMotion();
	~CMotion();

	static CMotion* Load(const char* pFilepath, CModel* pModel[], int& nNumModel, int nMaxMotion);
	void LoadModelInfo(FILE* pFile, char* aString, CModel* pModel[], int& nNumModel, int& nIdx);
	void LoadCharacterSet(FILE* pFile, char* aString, CModel* pModel[], int nNumModel, int parentIdx[]);
	void LoadMotionSet(FILE* pFile, char* aString, CMotion* pMotion, int& nCntMotion, int nMaxMotion);
	void Update(CModel** pModel, int& nNumModel);
	void StartBlendMotion(int  motionTypeBlend, int nFrameBlend);
	void SetMotion(int  motionType);

	bool IsCurrentMotionEnd(int motionType) const;
	int GetMotionType(void) { return m_motionType; }

	bool IsAttacking(void) const;

private:
	static constexpr int MAX_WORD	= 1024;	// �ő啶����
	static constexpr int MAX_PARTS	= 32;	// �ő�p�[�c��
	static constexpr int MAX_KEY	= 128;	// �ő�L�[��
	static constexpr int MAX_MOTION = 10;	// ���[�V�����̍ő吔

	void NormalizeRotX(D3DXVECTOR3 rot)
	{
		if (rot.x > D3DX_PI)
		{
			rot.x -= D3DX_PI * 2.0f;
		}
		else if (rot.x < -D3DX_PI)
		{
			rot.x += D3DX_PI * 2.0f;
		}
	}

	void NormalizeRotY(D3DXVECTOR3 rot)
	{
		if (rot.y > D3DX_PI)
		{
			rot.y -= D3DX_PI * 2.0f;
		}
		else if (rot.y < -D3DX_PI)
		{
			rot.y += D3DX_PI * 2.0f;
		}
	}

	void NormalizeRotZ(D3DXVECTOR3 rot)
	{
		if (rot.z > D3DX_PI)
		{
			rot.z -= D3DX_PI * 2.0f;
		}
		else if (rot.z < -D3DX_PI)
		{
			rot.z += D3DX_PI * 2.0f;
		}
	}

	//*************************************************************************
	// �L�[�\����
	//*************************************************************************
	typedef struct
	{
		float fPosX;							// �ʒu(X)
		float fPosY;							// �ʒu(Y)
		float fPosZ;							// �ʒu(Z)
		float fRotX;							// ����(X)
		float fRotY;							// ����(Y)
		float fRotZ;							// ����(Z)
	}KEY;

	//*************************************************************************
	// �L�[���̍\����
	//*************************************************************************
	typedef struct
	{
		int nFrame;								// �Đ��t���[��
		KEY aKey[MAX_PARTS];					// �e�p�[�c�̃L�[�v�f
	}KEY_INFO;

	//*************************************************************************
	// ���[�V�������̍\����
	//*************************************************************************
	typedef struct
	{
		bool bLoop;								// ���[�v���邩�ǂ���
		int nNumKey;							// �L�[�̑���
		KEY_INFO aKeyInfo[MAX_KEY];				// �L�[���
		int startKey, startFrame;
	}MOTION_INFO;

	MOTION_INFO m_aMotionInfo[MAX_MOTION];		// ���[�V�������
	int  m_motionType;							// ���[�V�����̎��
	int m_nNumMotion;							// ���[�V��������
	bool m_bLoopMotion;							// ���[�v���邩�ǂ���
	int m_nNumKey;								// �L�[�̑���
	int m_nKey;									// ���݂̃L�[No.
	int m_nCounterMotion;						// ���[�V�����̃J�E���^�[

	bool m_bFinishMotion;						// ���݂̃��[�V�������I�����Ă��邩�ǂ���
	bool m_bBlendMotion;						// �u�����h���[�V���������邩�ǂ���
	int  m_motionTypeBlend;						// �u�����h���[�V���������邩�ǂ���
	bool m_bLoopMotionBlend;					// �u�����h���[�V���������[�v���邩�ǂ���
	int m_nNumKeyBlend;							// �u�����h���[�V�����̃L�[��
	int m_nKeyBlend;							// �u�����h���[�V�����̌��݂̃L�[No.
	int m_nCounterMotionBlend;					// �u�����h���[�V�����̃J�E���^�[
	int m_nFrameBlend;							// �u�����h�̃t���[����(���t���[�������ău�����h���邩)
	int m_nCounterBlend;						// �u�����h�J�E���^�[

};

#endif