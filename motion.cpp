//=============================================================================
//
// モーション処理 [motion.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "motion.h"
#include "manager.h"
#include "player.h"

//=============================================================================
// コンストラクタ
//=============================================================================
CMotion::CMotion()
{
	// 値のクリア
	m_motionType		  = 0;				// モーションの種類
	m_nNumMotion		  = 0;				// モーション総数
	m_bLoopMotion		  = false;			// ループするかどうか
	m_nNumKey			  = 0;				// キーの総数
	m_nKey				  = 0;				// 現在のキーNo.
	m_nCounterMotion	  = 0;				// モーションのカウンター
	m_bFinishMotion		  = false;			// 現在のモーションが終了しているかどうか
	m_bBlendMotion		  = false;			// ブレンドモーションがあるかどうか
	m_motionTypeBlend	  = 0;				// ブレンドモーションの種類
	m_bLoopMotionBlend	  = false;			// ブレンドモーションがループするかどうか
	m_nNumKeyBlend        = 0;				// ブレンドモーションのキー数
	m_nKeyBlend           = 0;				// ブレンドモーションの現在のキーNo.
	m_nCounterMotionBlend = 0;				// ブレンドモーションのカウンター
	m_nFrameBlend		  = 0;				// ブレンドのフレーム数(何フレームかけてブレンドするか)
	m_nCounterBlend       = 0;				// ブレンドカウンター

}
//=============================================================================
// デストラクタ
//=============================================================================
CMotion::~CMotion()
{
	// なし
}
//=============================================================================
// モーションテキストファイルの読み込み
//=============================================================================
CMotion* CMotion::Load(const char* pFilepath, CModel* pModel[], int& nNumModel, int nMaxMotion)
{
	CMotion* pMotion = new CMotion;

	// ファイルを開く
	FILE* pFile = fopen(pFilepath, "r");

	if (!pFile)
	{
		return nullptr;  // ファイルが開けなかった
	}

	char aString[MAX_WORD];
	int nIdx = 0;

	int nCntMotion = 0;

	// 一時的な親のインデックス配列
	int parentIdx[MAX_PARTS];

	for (int i = 0; i < MAX_PARTS; i++) 
	{
		parentIdx[i] = -1;
	}

	while (fscanf(pFile, "%s", aString) != EOF)
	{
		if (strcmp(aString, "SCRIPT") != 0)
		{
			continue;
		}

		while (true)
		{
			if (fscanf(pFile, "%s", aString) == EOF)
			{
				break;
			}

			if (strcmp(aString, "NUM_MODEL") == 0 || strcmp(aString, "MODEL_FILENAME") == 0)
			{
				// モデル情報の読み込み処理
				pMotion->LoadModelInfo(pFile, aString, pModel, nNumModel, nIdx);
			}
			else if (strcmp(aString, "CHARACTERSET") == 0)
			{
				// キャラの設定処理
				pMotion->LoadCharacterSet(pFile, aString, pModel, nNumModel, parentIdx);
			}
			else if (strcmp(aString, "MOTIONSET") == 0)
			{
				// モーションの読み込み処理
				pMotion->LoadMotionSet(pFile, aString, pMotion, nCntMotion, nMaxMotion);
			}
			else if (strcmp(aString, "END_SCRIPT") == 0)
			{
				break;
			}
		}
	}

	// ファイルを閉じる
	fclose(pFile);

	// 親子関係設定
	for (int nCnt = 0; nCnt < nNumModel; nCnt++)
	{
		if (parentIdx[nCnt] >= 0 && parentIdx[nCnt] < nNumModel)
		{
			pModel[nCnt]->SetParent(pModel[parentIdx[nCnt]]);
		}
	}

	return pMotion;
}
//=============================================================================
// モデル情報の読み込み処理
//=============================================================================
void CMotion::LoadModelInfo(FILE* pFile, char* aString, CModel* pModel[], int& nNumModel, int& nIdx)
{
	if (strcmp(aString, "NUM_MODEL") == 0)
	{
		fscanf(pFile, "%s", aString); // "="

		if (strcmp(aString, "=") == 0)
		{
			fscanf(pFile, "%d", &nNumModel);
		}
	}
	else if (strcmp(aString, "MODEL_FILENAME") == 0)
	{
		fscanf(pFile, "%s", aString); // "="

		if (strcmp(aString, "=") == 0)
		{
			fscanf(pFile, "%s", aString);

			// モデルの生成
			pModel[nIdx] = CModel::Create(aString, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0));
			nIdx++;
		}
	}
}
//=============================================================================
// キャラの設定処理
//=============================================================================
void CMotion::LoadCharacterSet(FILE* pFile, char* aString, CModel* pModel[], int nNumModel, int parentIdx[])
{
	while (fscanf(pFile, "%s", aString) != EOF)
	{
		if (strcmp(aString, "END_CHARACTERSET") == 0)
		{
			break;
		}

		if (strcmp(aString, "PARTSSET") != 0)
		{
			continue;
		}

		// PARTSSET開始
		int idx = -1;
		int pIdx = -1;
		D3DXVECTOR3 pos(0, 0, 0);
		D3DXVECTOR3 rot(0, 0, 0);

		// PARTSSET内ループ
		while (fscanf(pFile, "%s", aString) != EOF)
		{
			if (strcmp(aString, "END_PARTSSET") == 0)
			{
				// データ設定
				if (idx >= 0 && idx < nNumModel && pModel[idx] != nullptr)
				{
					pModel[idx]->SetPos(pos);
					pModel[idx]->SetRot(rot);
					parentIdx[idx] = pIdx;
				}
				break;
			}

			if (strcmp(aString, "INDEX") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%d", &idx);
			}
			else if (strcmp(aString, "PARENT") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%d", &pIdx);
			}
			else if (strcmp(aString, "POS") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%f %f %f", &pos.x, &pos.y, &pos.z);
			}
			else if (strcmp(aString, "ROT") == 0)
			{
				fscanf(pFile, "%s", aString); // "="
				fscanf(pFile, "%f %f %f", &rot.x, &rot.y, &rot.z);
			}
		}
	}
}
//=============================================================================
// モーションの読み込み処理
//=============================================================================
void CMotion::LoadMotionSet(FILE* pFile, char* aString, CMotion* pMotion, int& nCntMotion, int nMaxMotion)
{
	if (nCntMotion >= nMaxMotion)
	{
		return;
	}

	int nCntKey = 0;

	while (fscanf(pFile, "%s", aString) != EOF)
	{
		if (strcmp(aString, "END_MOTIONSET") == 0)
		{
			nCntMotion++;
			break;
		}

		if (strcmp(aString, "NUM_KEY") != 0)
		{
			continue;
		}

		fscanf(pFile, "%s", aString); // "="
		fscanf(pFile, "%d", &pMotion->m_aMotionInfo[nCntMotion].nNumKey);

		// キー数分ループ
		for (nCntKey = 0; nCntKey < pMotion->m_aMotionInfo[nCntMotion].nNumKey; nCntKey++)
		{
			while (fscanf(pFile, "%s", aString) != EOF)
			{
				if (strcmp(aString, "KEYSET") == 0)
				{
					break;
				}
			}

			// フレーム読み込み
			while (fscanf(pFile, "%s", aString) != EOF)
			{
				if (strcmp(aString, "FRAME") == 0)
				{
					fscanf(pFile, "%s", aString); // "="
					fscanf(pFile, "%d", &pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].nFrame);
					break;
				}
			}

			int nCntPos = 0;
			int nCntRot = 0;

			// KEY内ループ
			while (fscanf(pFile, "%s", aString) != EOF)
			{
				if (strcmp(aString, "END_KEYSET") == 0)
				{
					break;
				}

				if (strcmp(aString, "KEY") != 0)
				{
					continue;
				}

				while (fscanf(pFile, "%s", aString) != EOF)
				{
					if (strcmp(aString, "END_KEY") == 0)
					{
						break;
					}

					if (strcmp(aString, "POS") == 0)
					{
						fscanf(pFile, "%s", aString); // "="
						fscanf(pFile, "%f %f %f",
							&pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPos].fPosX,
							&pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPos].fPosY,
							&pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntPos].fPosZ);
						nCntPos++;
					}
					else if (strcmp(aString, "ROT") == 0)
					{
						fscanf(pFile, "%s", aString); // "="
						fscanf(pFile, "%f %f %f",
							&pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRot].fRotX,
							&pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRot].fRotY,
							&pMotion->m_aMotionInfo[nCntMotion].aKeyInfo[nCntKey].aKey[nCntRot].fRotZ);
						nCntRot++;
					}
				} // END while KEY
			} // END while KEYSET
		} // END for NUM_KEY
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CMotion::Update(CModel** pModel, int& nNumModel)
{
	// ブレンド中かどうか
	if (m_bBlendMotion && m_nFrameBlend > 0 && m_nCounterBlend < m_nFrameBlend)
	{
		// ブレンド進行度
		float blendRate = (float)m_nCounterBlend / (float)m_nFrameBlend;

		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{
			// 現在モーションの補間結果
			int nNextKey = (m_nKey + 1) % m_aMotionInfo[m_motionType].nNumKey;

			float tCurrent = (float)m_nCounterMotion / m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame;

			KEY keyCur = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel];
			KEY keyNext = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel];

			D3DXVECTOR3 posCurrent =
			{
				keyCur.fPosX + (keyNext.fPosX - keyCur.fPosX) * tCurrent,
				keyCur.fPosY + (keyNext.fPosY - keyCur.fPosY) * tCurrent,
				keyCur.fPosZ + (keyNext.fPosZ - keyCur.fPosZ) * tCurrent,
			};

			D3DXVECTOR3 rotCurrent =
			{
				keyCur.fRotX + (keyNext.fRotX - keyCur.fRotX) * tCurrent,
				keyCur.fRotY + (keyNext.fRotY - keyCur.fRotY) * tCurrent,
				keyCur.fRotZ + (keyNext.fRotZ - keyCur.fRotZ) * tCurrent,
			};

			// 正規化
			NormalizeRotX(rotCurrent);
			NormalizeRotY(rotCurrent);
			NormalizeRotZ(rotCurrent);

			// ブレンドモーションの補間結果
			int nNextKeyBlend = (m_nKeyBlend + 1) % m_aMotionInfo[m_motionTypeBlend].nNumKey;
			float tBlend = (float)m_nCounterMotionBlend / m_aMotionInfo[m_motionTypeBlend].aKeyInfo[m_nKeyBlend].nFrame;

			KEY keyCurBlend = m_aMotionInfo[m_motionTypeBlend].aKeyInfo[m_nKeyBlend].aKey[nCntModel];
			KEY keyNextBlend = m_aMotionInfo[m_motionTypeBlend].aKeyInfo[nNextKeyBlend].aKey[nCntModel];

			D3DXVECTOR3 posBlend = 
			{
				keyCurBlend.fPosX + (keyNextBlend.fPosX - keyCurBlend.fPosX) * tBlend,
				keyCurBlend.fPosY + (keyNextBlend.fPosY - keyCurBlend.fPosY) * tBlend,
				keyCurBlend.fPosZ + (keyNextBlend.fPosZ - keyCurBlend.fPosZ) * tBlend,
			};

			D3DXVECTOR3 rotBlend =
			{
				keyCurBlend.fRotX + (keyNextBlend.fRotX - keyCurBlend.fRotX) * tBlend,
				keyCurBlend.fRotY + (keyNextBlend.fRotY - keyCurBlend.fRotY) * tBlend,
				keyCurBlend.fRotZ + (keyNextBlend.fRotZ - keyCurBlend.fRotZ) * tBlend,
			};

			// 最終的に現在モーションとブレンドモーションをさらに補間
			D3DXVECTOR3 posFinal = posCurrent * (1.0f - blendRate) + posBlend * blendRate;
			D3DXVECTOR3 rotFinal = rotCurrent * (1.0f - blendRate) + rotBlend * blendRate;

			// 全パーツの位置・向きを設定
			pModel[nCntModel]->SetOffsetPos(posFinal);
			pModel[nCntModel]->SetOffsetRot(rotFinal);
		}

		// モーションカウンター進行
		m_nCounterMotion++;
		if (m_nCounterMotion >= m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame)
		{
			m_nCounterMotion = 0;
			m_nKey++;

			if (m_nKey >= m_aMotionInfo[m_motionType].nNumKey)
			{
				m_nKey = 0;

				//m_bFinishMotion = true;
			}
		}

		// モーションブレンドカウンター進行
		m_nCounterMotionBlend++;
		if (m_nCounterMotionBlend >= m_aMotionInfo[m_motionTypeBlend].aKeyInfo[m_nKeyBlend].nFrame)
		{
			m_nCounterMotionBlend = 0;
			m_nKeyBlend++;

			if (m_nKeyBlend >= m_aMotionInfo[m_motionTypeBlend].nNumKey)
			{
				m_nKeyBlend = 0;

				//m_bFinishMotion = true;
			}
		}

		m_nCounterBlend++;

		// ブレンド終了判定
		if (m_nCounterBlend >= m_nFrameBlend)
		{
			// ブレンド終了。ターゲットモーションへ切り替え
			m_motionType = m_motionTypeBlend;
			m_nKey = m_nKeyBlend;
			m_nCounterMotion = m_nCounterMotionBlend;

			m_bBlendMotion = false;

			m_bFinishMotion = false;
		}
	}
	else
	{
		// ブレンド無しの通常モーション更新
		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{
			int nNextKey = (m_nKey + 1) % m_aMotionInfo[m_motionType].nNumKey;

			// 境界チェック
			if (m_nKey >= m_aMotionInfo[m_motionType].nNumKey || nNextKey >= m_aMotionInfo[m_motionType].nNumKey)
			{
				m_nKey = 0;
			}

			D3DXVECTOR3 Mpos, Mrot;
			D3DXVECTOR3 MAnswer, MAnswer2;	// 計算結果用変数

			// キー情報から位置・向きを算出
			Mpos.x = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fPosX - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX;
			Mpos.y = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fPosY - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY;
			Mpos.z = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fPosZ - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ;

			Mrot.x = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fRotX - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX;
			Mrot.y = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fRotY - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY;
			Mrot.z = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fRotZ - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ;

			// 正規化
			NormalizeRotX(Mrot);
			NormalizeRotY(Mrot);
			NormalizeRotZ(Mrot);

			// 補間係数を計算
			float t = (float)m_nCounterMotion / m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame;

			// 求める値
			MAnswer.x = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX + Mpos.x * t;
			MAnswer.y = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY + Mpos.y * t;
			MAnswer.z = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ + Mpos.z * t;

			MAnswer2.x = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX + Mrot.x * t;
			MAnswer2.y = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY + Mrot.y * t;
			MAnswer2.z = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ + Mrot.z * t;

			// 全パーツの位置・向きを設定
			pModel[nCntModel]->SetOffsetPos(MAnswer);
			pModel[nCntModel]->SetOffsetRot(MAnswer2);
		}

		m_nCounterMotion++;		// 再生フレーム数に達したら現在のキーを1つ進める

		if (m_nCounterMotion >= m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame)
		{
			m_nCounterMotion = 0;
			m_nKey++;

			if (m_nKey >= m_aMotionInfo[m_motionType].nNumKey)
			{
				m_nKey = 0;

				m_bFinishMotion = true;
			}
		}
	}
}
//=============================================================================
// モーションブレンド開始処理
//=============================================================================
void CMotion::StartBlendMotion(int  motionTypeBlend, int nFrameBlend)
{
	m_motionTypeBlend = motionTypeBlend;
	m_nFrameBlend = nFrameBlend;
	m_nCounterBlend = 0;

	// 先頭からスタート
	m_nKeyBlend = 0;
	m_nCounterMotionBlend = 0;

	m_bBlendMotion = true;

	// ブレンド先のモーションキー数取得
	m_nNumKeyBlend = m_aMotionInfo[m_motionTypeBlend].nNumKey;

	// ブレンド先モーションはループ設定とする
	m_bLoopMotionBlend = false;

	m_bFinishMotion = false;
}
//=============================================================================
// モーションの設定処理
//=============================================================================
void CMotion::SetMotion(int  motionType)
{
	m_motionType = motionType;
	m_nKey = 0;
	m_nCounterMotion = 0;
	m_bFinishMotion = false;
}
//=============================================================================
// モーションの終了判定
//=============================================================================
bool CMotion::IsCurrentMotionEnd(int motionType) const
{
	// モーションタイプが一致する場合のみ終了判定
	return (m_motionType == motionType) && m_bFinishMotion;
}
//=============================================================================
// 攻撃中かどうか
//=============================================================================
bool CMotion::IsAttacking(void) const
{
	return m_motionType == CPlayer::ATTACK_01 && !m_bFinishMotion;
}