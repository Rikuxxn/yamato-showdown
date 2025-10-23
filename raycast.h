//=============================================================================
//
// ���C�L���X�g���� [raycast.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RAYCAST_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RAYCAST_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************

//*****************************************************************************
// ���C�L���X�g�N���X
//*****************************************************************************
class CRayCast
{
public:

    // �}�E�X�ʒu���烏�[���h���C���擾
    static bool GetMouseRay(D3DXVECTOR3& outOrigin, D3DXVECTOR3& outDir);

    // ���C��AABB�̓����蔻��
    static bool IntersectAABB(const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        const D3DXVECTOR3& boxMin,
        const D3DXVECTOR3& boxMax,
        float& outDist);

    // ���C��OBB�̓����蔻��
    static bool IntersectOBB(
        const D3DXVECTOR3& rayOrigin,
        const D3DXVECTOR3& rayDir,
        const D3DXMATRIX& worldMatrix,
        const D3DXVECTOR3& halfSize,
        float& outDistance
    );

};

#endif