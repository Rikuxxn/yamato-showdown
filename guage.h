//=============================================================================
//
// �Q�[�W���� [guage.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GUAGE_H_
#define _GUAGE_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object2D.h"

// �O���錾
class CCharacter;

//*****************************************************************************
// �Q�[�W�N���X
//*****************************************************************************
class CGuage : public CObject2D
{
public:
	// ���
	enum GUAGETYPE
	{
		TYPE_NONE = 0,	// �������Ă��Ȃ����
		TYPE_GUAGE,		// �΃Q�[�W
		TYPE_BACKGUAGE,	// �o�b�N�Q�[�W(��)
		TYPE_FRAME,		// �g
		TYPE_MAX
	};

	CGuage();
	~CGuage();

	static CGuage* Create(GUAGETYPE type, D3DXVECTOR3 pos, float fWidth, float fHeight);
	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);
	void SetTargetCharacter(CCharacter* pChar) { m_pTargetChar = pChar; }

private:
	int m_nIdxTexture;			// �e�N�X�`���C���f�b�N�X
	GUAGETYPE m_type;			// �Q�[�W�̎��
	float m_fWidth;				// ����
	float m_fHeight;			// �c��
	float m_targetRate;			// ���ۂ�HP����
	float m_currentRate;		// �\���p�Q�[�W�����i�Ǐ]�p�j
	float m_speed;				// �Ǐ]���x
	float m_delayTimer;			// �x���^�C�}�[(�o�b�N�Q�[�W�p)
	CCharacter* m_pTargetChar;	// ���̃Q�[�W���Ǐ]����L�����N�^�[
};

#endif
