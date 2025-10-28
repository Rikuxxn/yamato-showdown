//=============================================================================
//
// ����̓����蔻�菈�� [weaponcollider.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _WEAPONCOLLIDER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _WEAPONCOLLIDER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "enemy.h"
#include "collisionUtils.h"

//*****************************************************************************
// ����R���C�_�[�N���X
//*****************************************************************************
class CWeaponCollider
{
public:
    CWeaponCollider();
    ~CWeaponCollider();

    void SetActive(bool active) { m_bActive = active; }
    bool IsActive(void) const { return m_bActive; }
    void ResetHit(void) { m_bHasHit = false; }
    void ResetPrevPos(void) { m_prevTip = m_currTip; m_prevBase = m_currBase; }

    D3DXVECTOR3 GetCurrentBasePos(void) { return m_currBase; }
    D3DXVECTOR3 GetCurrentTipPos(void) { return m_currTip; }

    // �����蔻��̍X�V����
    void Update(CModel* pWeapon)
    {
        // �e�q�K�w���݂̃��[���h�s����擾
        D3DXMATRIX worldMatrix = pWeapon->GetMtxWorld();

        // ���̍����Ɛ�[�I�t�Z�b�g�i���[�J�����W�j
        D3DXVECTOR3 localTip(0, 40, 0); // ��[
        D3DXVECTOR3 localBase(0, 10, 0); // ����

        // ���[�J�������[���h�ϊ�
        D3DXVec3TransformCoord(&m_currTip, &localTip, &worldMatrix);
        D3DXVec3TransformCoord(&m_currBase, &localBase, &worldMatrix);
    }

    // �����蔻�菈��
    void CheckHit(CEnemy* pEnemy)
    {
        if (!m_bActive || m_bHasHit || !pEnemy)
        {
            return;
        }

        // �J�v�Z���̏㉺�[�_
        D3DXVECTOR3 top = pEnemy->GetPos() + D3DXVECTOR3(0, pEnemy->GetHeight() * 0.5f, 0);
        D3DXVECTOR3 bottom = pEnemy->GetPos() - D3DXVECTOR3(0, pEnemy->GetHeight() * 0.5f, 0);
        float radius = pEnemy->GetRadius();

        float weaponRadius = 15.0f; // �����蔻��̔��a

        if (CCollision::IntersectSegmentCapsule(m_prevBase, m_currBase, bottom, top, radius + weaponRadius) ||
            CCollision::IntersectSegmentCapsule(m_prevTip, m_currTip, bottom, top, radius + weaponRadius) ||
            CCollision::IntersectSegmentCapsule(m_prevBase, m_currTip, bottom, top, radius + weaponRadius))
        {
            // �_���[�W����
            pEnemy->Damage(5.0f);
            m_bHasHit = true; // ��������
        }

        // ���W�X�V
        m_prevBase = m_currBase;
        m_prevTip = m_currTip;
    }

private:
    D3DXVECTOR3 m_prevBase;
    D3DXVECTOR3 m_prevTip;
    D3DXVECTOR3 m_currBase;
    D3DXVECTOR3 m_currTip;
    bool m_bActive;
    bool m_bHasHit;
};

#endif

