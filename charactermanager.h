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
#include "manager.h"

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

    // �����蔻��̐���
    void CreatePhysics(float radius, float height);

    // Physics�̔j��
    void ReleasePhysics(void);

    // �����蔻��̈ʒu�X�V
    void UpdateCollider(D3DXVECTOR3 offset);

    // ������ԏ���
    void UpdateRotation(float fInterpolationSpeed);

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
        if (m_isDead)
        {
            return; // ���łɎ���ł���Ώ������Ȃ�
        }

        m_fHp -= fDamage;

        if (m_fHp <= 0.0f)
        {
            m_isDead = true;// ����
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

    //*****************************************************************************
    // flagment�֐�
    //*****************************************************************************
    bool IsDead(void) { return m_isDead; }

    //*****************************************************************************
    // setter�֐�
    //*****************************************************************************
    void SetHp(float fHp) { m_fHp = fHp; m_fMaxHp = m_fHp; }
    void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
    void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
    void SetRotDest(const D3DXVECTOR3& rotDest) { m_rotDest = rotDest; }

    //*****************************************************************************
    // getter�֐�
    //*****************************************************************************
    D3DXVECTOR3 GetPos(void) { return m_pos; }
    D3DXVECTOR3 GetRot(void) { return m_rot; };
    const D3DXVECTOR3& GetRotDest(void) const { return m_rotDest; }
    D3DXVECTOR3 GetSize(void) { return m_size; }
    float GetHp(void) const { return m_fHp; }
    float GetMaxHp(void) const { return m_fMaxHp; }
    btScalar GetRadius(void) const { return m_radius; }
    btScalar GetHeight(void) const { return m_height; }
    btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBody�̎擾
    btCollisionShape* GetCollisionShape(void) { return m_pShape; }
    D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }

private:

	float m_fHp;                // HP
	float m_fMaxHp;             // HP�ő��
    bool m_isDead;              // ���񂾂��ǂ���
	CGuage* m_pHpGuage;			// �΃Q�[�W
	CGuage* m_pBackGuage;		// �ԃQ�[�W�i�x��ĒǏ]�j
	CGuage* m_pFrame;			// �g
    D3DXVECTOR3 m_pos;			// �ʒu
    D3DXVECTOR3 m_rot;			// ����
    D3DXVECTOR3 m_rotDest;		// ����
    D3DXVECTOR3 m_size;			// �T�C�Y
    btRigidBody* m_pRigidBody;	// ���̂ւ̃|�C���^
    btCollisionShape* m_pShape;	// �����蔻��̌`�ւ̃|�C���^
    btScalar m_radius;			// �J�v�Z���R���C�_�[�̔��a
    btScalar m_height;			// �J�v�Z���R���C�_�[�̍���
    D3DXVECTOR3 m_colliderPos;	// �J�v�Z���R���C�_�[�̈ʒu

};

//*****************************************************************************
// �L�����N�^�[�}�l�[�W���[�N���X
//*****************************************************************************
class CCharacterManager
{
public:
    //// �C���X�^���X�̎擾
    //static CCharacterManager& GetInstance(void)
    //{
    //    static CCharacterManager instance;
    //    return instance;
    //}

    // �L�����N�^�[�ǉ�����
    void AddCharacter(CCharacter* pChar)
    {
        // ���X�g�ɒǉ�����
        m_characters.push_back(pChar);
    }

    //// �L�����N�^�[�̎擾����
    //template <class characterType>
    //characterType* GetCharacter(void)
    //{
    //    for (const auto& c : m_Characters)
    //    {
    //        if (auto casted = dynamic_cast<characterType*>(c))
    //        {
    //            return casted;
    //        }
    //    }
    //    return nullptr;
    //}

    void Init(void)
    {
        for (auto chara : m_characters)
        {
            // �L�����N�^�[�̍X�V����
            chara->Init();
        }
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
    std::vector<CCharacter*> m_characters;// �L�����N�^�[�̃��X�g
};

#endif

