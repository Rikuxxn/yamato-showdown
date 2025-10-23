//=============================================================================
//
// ���[�V�������� [motion.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "motion.h"
#include "manager.h"
#include "player.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMotion::CMotion()
{
	// �l�̃N���A
	m_motionType		  = 0;				// ���[�V�����̎��
	m_nNumMotion		  = 0;				// ���[�V��������
	m_bLoopMotion		  = false;			// ���[�v���邩�ǂ���
	m_nNumKey			  = 0;				// �L�[�̑���
	m_nKey				  = 0;				// ���݂̃L�[No.
	m_nCounterMotion	  = 0;				// ���[�V�����̃J�E���^�[
	m_bFinishMotion		  = false;			// ���݂̃��[�V�������I�����Ă��邩�ǂ���
	m_bBlendMotion		  = false;			// �u�����h���[�V���������邩�ǂ���
	m_motionTypeBlend	  = 0;				// �u�����h���[�V�����̎��
	m_bLoopMotionBlend	  = false;			// �u�����h���[�V���������[�v���邩�ǂ���
	m_nNumKeyBlend        = 0;				// �u�����h���[�V�����̃L�[��
	m_nKeyBlend           = 0;				// �u�����h���[�V�����̌��݂̃L�[No.
	m_nCounterMotionBlend = 0;				// �u�����h���[�V�����̃J�E���^�[
	m_nFrameBlend		  = 0;				// �u�����h�̃t���[����(���t���[�������ău�����h���邩)
	m_nCounterBlend       = 0;				// �u�����h�J�E���^�[

}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMotion::~CMotion()
{
	// �Ȃ�
}
//=============================================================================
// ���[�V�����e�L�X�g�t�@�C���̓ǂݍ���
//=============================================================================
CMotion* CMotion::Load(const char* pFilepath, CModel* pModel[], int& nNumModel, int nMaxMotion)
{
	CMotion* pMotion = new CMotion;

	// �t�@�C�����J��
	FILE* pFile = fopen(pFilepath, "r");

	if (!pFile)
	{
		return nullptr;  // �t�@�C�����J���Ȃ�����
	}

	char aString[MAX_WORD];
	int nIdx = 0;

	int nCntMotion = 0;

	// �ꎞ�I�Ȑe�̃C���f�b�N�X�z��
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
				// ���f�����̓ǂݍ��ݏ���
				pMotion->LoadModelInfo(pFile, aString, pModel, nNumModel, nIdx);
			}
			else if (strcmp(aString, "CHARACTERSET") == 0)
			{
				// �L�����̐ݒ菈��
				pMotion->LoadCharacterSet(pFile, aString, pModel, nNumModel, parentIdx);
			}
			else if (strcmp(aString, "MOTIONSET") == 0)
			{
				// ���[�V�����̓ǂݍ��ݏ���
				pMotion->LoadMotionSet(pFile, aString, pMotion, nCntMotion, nMaxMotion);
			}
			else if (strcmp(aString, "END_SCRIPT") == 0)
			{
				break;
			}
		}
	}

	// �t�@�C�������
	fclose(pFile);

	// �e�q�֌W�ݒ�
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
// ���f�����̓ǂݍ��ݏ���
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

			// ���f���̐���
			pModel[nIdx] = CModel::Create(aString, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(0, 0, 0));
			nIdx++;
		}
	}
}
//=============================================================================
// �L�����̐ݒ菈��
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

		// PARTSSET�J�n
		int idx = -1;
		int pIdx = -1;
		D3DXVECTOR3 pos(0, 0, 0);
		D3DXVECTOR3 rot(0, 0, 0);

		// PARTSSET�����[�v
		while (fscanf(pFile, "%s", aString) != EOF)
		{
			if (strcmp(aString, "END_PARTSSET") == 0)
			{
				// �f�[�^�ݒ�
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
// ���[�V�����̓ǂݍ��ݏ���
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

		// �L�[�������[�v
		for (nCntKey = 0; nCntKey < pMotion->m_aMotionInfo[nCntMotion].nNumKey; nCntKey++)
		{
			while (fscanf(pFile, "%s", aString) != EOF)
			{
				if (strcmp(aString, "KEYSET") == 0)
				{
					break;
				}
			}

			// �t���[���ǂݍ���
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

			// KEY�����[�v
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
// �X�V����
//=============================================================================
void CMotion::Update(CModel** pModel, int& nNumModel)
{
	// �u�����h�����ǂ���
	if (m_bBlendMotion && m_nFrameBlend > 0 && m_nCounterBlend < m_nFrameBlend)
	{
		// �u�����h�i�s�x
		float blendRate = (float)m_nCounterBlend / (float)m_nFrameBlend;

		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{
			// ���݃��[�V�����̕�Ԍ���
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

			// ���K��
			NormalizeRotX(rotCurrent);
			NormalizeRotY(rotCurrent);
			NormalizeRotZ(rotCurrent);

			// �u�����h���[�V�����̕�Ԍ���
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

			// �ŏI�I�Ɍ��݃��[�V�����ƃu�����h���[�V����������ɕ��
			D3DXVECTOR3 posFinal = posCurrent * (1.0f - blendRate) + posBlend * blendRate;
			D3DXVECTOR3 rotFinal = rotCurrent * (1.0f - blendRate) + rotBlend * blendRate;

			// �S�p�[�c�̈ʒu�E������ݒ�
			pModel[nCntModel]->SetOffsetPos(posFinal);
			pModel[nCntModel]->SetOffsetRot(rotFinal);
		}

		// ���[�V�����J�E���^�[�i�s
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

		// ���[�V�����u�����h�J�E���^�[�i�s
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

		// �u�����h�I������
		if (m_nCounterBlend >= m_nFrameBlend)
		{
			// �u�����h�I���B�^�[�Q�b�g���[�V�����֐؂�ւ�
			m_motionType = m_motionTypeBlend;
			m_nKey = m_nKeyBlend;
			m_nCounterMotion = m_nCounterMotionBlend;

			m_bBlendMotion = false;

			m_bFinishMotion = false;
		}
	}
	else
	{
		// �u�����h�����̒ʏ탂�[�V�����X�V
		for (int nCntModel = 0; nCntModel < nNumModel; nCntModel++)
		{
			int nNextKey = (m_nKey + 1) % m_aMotionInfo[m_motionType].nNumKey;

			// ���E�`�F�b�N
			if (m_nKey >= m_aMotionInfo[m_motionType].nNumKey || nNextKey >= m_aMotionInfo[m_motionType].nNumKey)
			{
				m_nKey = 0;
			}

			D3DXVECTOR3 Mpos, Mrot;
			D3DXVECTOR3 MAnswer, MAnswer2;	// �v�Z���ʗp�ϐ�

			// �L�[��񂩂�ʒu�E�������Z�o
			Mpos.x = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fPosX - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX;
			Mpos.y = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fPosY - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY;
			Mpos.z = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fPosZ - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ;

			Mrot.x = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fRotX - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX;
			Mrot.y = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fRotY - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY;
			Mrot.z = m_aMotionInfo[m_motionType].aKeyInfo[nNextKey].aKey[nCntModel].fRotZ - m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ;

			// ���K��
			NormalizeRotX(Mrot);
			NormalizeRotY(Mrot);
			NormalizeRotZ(Mrot);

			// ��ԌW�����v�Z
			float t = (float)m_nCounterMotion / m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].nFrame;

			// ���߂�l
			MAnswer.x = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosX + Mpos.x * t;
			MAnswer.y = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosY + Mpos.y * t;
			MAnswer.z = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fPosZ + Mpos.z * t;

			MAnswer2.x = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotX + Mrot.x * t;
			MAnswer2.y = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotY + Mrot.y * t;
			MAnswer2.z = m_aMotionInfo[m_motionType].aKeyInfo[m_nKey].aKey[nCntModel].fRotZ + Mrot.z * t;

			// �S�p�[�c�̈ʒu�E������ݒ�
			pModel[nCntModel]->SetOffsetPos(MAnswer);
			pModel[nCntModel]->SetOffsetRot(MAnswer2);
		}

		m_nCounterMotion++;		// �Đ��t���[�����ɒB�����猻�݂̃L�[��1�i�߂�

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
// ���[�V�����u�����h�J�n����
//=============================================================================
void CMotion::StartBlendMotion(int  motionTypeBlend, int nFrameBlend)
{
	m_motionTypeBlend = motionTypeBlend;
	m_nFrameBlend = nFrameBlend;
	m_nCounterBlend = 0;

	// �擪����X�^�[�g
	m_nKeyBlend = 0;
	m_nCounterMotionBlend = 0;

	m_bBlendMotion = true;

	// �u�����h��̃��[�V�����L�[���擾
	m_nNumKeyBlend = m_aMotionInfo[m_motionTypeBlend].nNumKey;

	// �u�����h�惂�[�V�����̓��[�v�ݒ�Ƃ���
	m_bLoopMotionBlend = false;

	m_bFinishMotion = false;
}
//=============================================================================
// ���[�V�����̐ݒ菈��
//=============================================================================
void CMotion::SetMotion(int  motionType)
{
	m_motionType = motionType;
	m_nKey = 0;
	m_nCounterMotion = 0;
	m_bFinishMotion = false;
}
//=============================================================================
// ���[�V�����̏I������
//=============================================================================
bool CMotion::IsCurrentMotionEnd(int motionType) const
{
	// ���[�V�����^�C�v����v����ꍇ�̂ݏI������
	return (m_motionType == motionType) && m_bFinishMotion;
}
//=============================================================================
// �U�������ǂ���
//=============================================================================
bool CMotion::IsAttacking(void) const
{
	return m_motionType == CPlayer::ATTACK_01 && !m_bFinishMotion;
}