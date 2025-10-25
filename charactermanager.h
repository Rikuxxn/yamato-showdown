//=============================================================================
//
// �L�����N�^�[�}�l�[�W���[���� [charactermanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CHARACTERMANAGER_H_
#define _CHARACTERMANAGER_H_

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "object.h"
#include "guage.h"

// ���O���std�̎g�p
using namespace std;

//*****************************************************************************
// �L�����N�^�[�N���X
//*****************************************************************************
class CCharacter : public CObject
{
public:
	CCharacter(int nPriority = 2);
	~CCharacter();

	HRESULT Init(void) = 0;
	void Uninit(void) = 0;
	void Update(void) = 0;
	void Draw(void) = 0;

	void SetHp(float fHp) { m_fHp = fHp; m_fMaxHp = m_fHp; }
	float GetHp(void) const { return m_fHp; }
	float GetMaxHp(void) const { return m_fMaxHp; }

    // HP�Q�[�W�̐ݒ菈��
	void SetGuages(D3DXVECTOR3 pos, float fWidth, float fHeight)
	{
        m_pFrame = CGuage::Create(CGuage::TYPE_FRAME, pos, fWidth, fHeight);// �g
        m_pBackGuage = CGuage::Create(CGuage::TYPE_BACKGUAGE, pos, fWidth, fHeight);// �o�b�N�Q�[�W
		m_pHpGuage = CGuage::Create(CGuage::TYPE_GUAGE, pos, fWidth, fHeight);// HP�Q�[�W

        // HP���ʊǗ����邽�߂ɃQ�[�W�̃^�[�Q�b�g��ݒ�
        m_pFrame->SetTargetCharacter(this);
        m_pBackGuage->SetTargetCharacter(this);
        m_pHpGuage->SetTargetCharacter(this);
	}

    // �_���[�W����
    void Damage(float fDamage)
    {
        m_fHp -= fDamage;

        if (m_fHp <= 0.0f)
        {
            m_fHp = 0.0f;
        }
    }

    // �񕜏���
    void Heal(float fHealAmount)
    {
        m_fHp += fHealAmount;

        if (m_fHp >= m_fMaxHp)
        {
            m_fHp = m_fMaxHp;
        }
    }

private:

	float m_fHp;
	float m_fMaxHp;

	CGuage* m_pHpGuage;			// �΃Q�[�W
	CGuage* m_pBackGuage;		// �ԃQ�[�W�i�x��ĒǏ]�j
	CGuage* m_pFrame;			// �g
};

//*****************************************************************************
// �L�����N�^�[�}�l�[�W���[�N���X
//*****************************************************************************
class CCharacterManager
{
public:
    // �L�����N�^�[�ǉ�����
    void AddCharacter(CCharacter* pChar)
    {
        // ���X�g�ɒǉ�����
        m_characters.push_back(pChar);
    }

    void Update(void)
    {
        for (auto chara : m_characters)
        {
            // �L�����N�^�[�̍X�V����
            chara->Update();
        }
    }

    void Draw(void)
    {
        for (auto chara : m_characters)
        {
            // �L�����N�^�[�̕`�揈��
            chara->Draw();
        }
    }

    void Uninit(void)
    {
        for (auto chara : m_characters)
        {
            // �L�����N�^�[�̏I������
            chara->Uninit();
            delete chara;
        }

        // ���X�g�̃N���A(��ɂ���)
        m_characters.clear();
    }

private:
    std::vector<CCharacter*> m_characters;
};

#endif

