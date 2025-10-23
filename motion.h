//=============================================================================
//
// モーション処理 [motion.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _MOTION_H_// このマクロ定義がされていなかったら
#define _MOTION_H_// 2重インクルード防止のマクロ定義

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "model.h"

//*****************************************************************************
// モーションクラス
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
	static constexpr int MAX_WORD	= 1024;	// 最大文字数
	static constexpr int MAX_PARTS	= 32;	// 最大パーツ数
	static constexpr int MAX_KEY	= 128;	// 最大キー数
	static constexpr int MAX_MOTION = 10;	// モーションの最大数

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
	// キー構造体
	//*************************************************************************
	typedef struct
	{
		float fPosX;							// 位置(X)
		float fPosY;							// 位置(Y)
		float fPosZ;							// 位置(Z)
		float fRotX;							// 向き(X)
		float fRotY;							// 向き(Y)
		float fRotZ;							// 向き(Z)
	}KEY;

	//*************************************************************************
	// キー情報の構造体
	//*************************************************************************
	typedef struct
	{
		int nFrame;								// 再生フレーム
		KEY aKey[MAX_PARTS];					// 各パーツのキー要素
	}KEY_INFO;

	//*************************************************************************
	// モーション情報の構造体
	//*************************************************************************
	typedef struct
	{
		bool bLoop;								// ループするかどうか
		int nNumKey;							// キーの総数
		KEY_INFO aKeyInfo[MAX_KEY];				// キー情報
		int startKey, startFrame;
	}MOTION_INFO;

	MOTION_INFO m_aMotionInfo[MAX_MOTION];		// モーション情報
	int  m_motionType;							// モーションの種類
	int m_nNumMotion;							// モーション総数
	bool m_bLoopMotion;							// ループするかどうか
	int m_nNumKey;								// キーの総数
	int m_nKey;									// 現在のキーNo.
	int m_nCounterMotion;						// モーションのカウンター

	bool m_bFinishMotion;						// 現在のモーションが終了しているかどうか
	bool m_bBlendMotion;						// ブレンドモーションがあるかどうか
	int  m_motionTypeBlend;						// ブレンドモーションがあるかどうか
	bool m_bLoopMotionBlend;					// ブレンドモーションがループするかどうか
	int m_nNumKeyBlend;							// ブレンドモーションのキー数
	int m_nKeyBlend;							// ブレンドモーションの現在のキーNo.
	int m_nCounterMotionBlend;					// ブレンドモーションのカウンター
	int m_nFrameBlend;							// ブレンドのフレーム数(何フレームかけてブレンドするか)
	int m_nCounterBlend;						// ブレンドカウンター

};

#endif