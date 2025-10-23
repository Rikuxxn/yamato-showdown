//=============================================================================
//
// ���C�L���X�g���� [raycast.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "raycast.h"
#include "renderer.h"
#include "manager.h"

// ���O���std�̎g�p
using namespace std;

//=============================================================================
// �}�E�X�ʒu���烏�[���h���C���擾���鏈��
//=============================================================================
bool CRayCast::GetMouseRay(D3DXVECTOR3& outOrigin, D3DXVECTOR3& outDir)
{
    // �}�E�X���W�擾
    POINT pt;
    GetCursorPos(&pt);

    // �N���C�A���g���W�ɕϊ�
    HWND hwnd = GetActiveWindow();
    ScreenToClient(hwnd, &pt);

    // �f�o�C�X�E�r���[�|�[�g
    CRenderer* renderer = CManager::GetRenderer();
    LPDIRECT3DDEVICE9 pDevice = renderer->GetDevice();

    D3DVIEWPORT9 vp;
    pDevice->GetViewport(&vp);

    // �s��擾
    D3DXMATRIX matProj, matView, matWorld;
    pDevice->GetTransform(D3DTS_PROJECTION, &matProj);
    pDevice->GetTransform(D3DTS_VIEW, &matView);
    D3DXMatrixIdentity(&matWorld); // �P�ʍs��

    // Unproject�� near/far ��2�_�����[���h��Ԃɕϊ�
    D3DXVECTOR3 nearPt((float)pt.x, (float)pt.y, 0.0f);
    D3DXVECTOR3 farPt((float)pt.x, (float)pt.y, 1.0f);

    D3DXVECTOR3 worldNear, worldFar;
    D3DXVec3Unproject(&worldNear, &nearPt, &vp, &matProj, &matView, &matWorld);
    D3DXVec3Unproject(&worldFar, &farPt, &vp, &matProj, &matView, &matWorld);

    // ���C�̏o���_�ƕ���
    outOrigin = worldNear;
    outDir = worldFar - worldNear;
    D3DXVec3Normalize(&outDir, &outDir);

    return true;
}
//=============================================================================
// ���C��AABB�̓����蔻��
//=============================================================================
bool CRayCast::IntersectAABB(const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    const D3DXVECTOR3& boxMin,
    const D3DXVECTOR3& boxMax,
    float& outDist)
{
    const float EPSILON = 1e-6f;
    float tMin = 0.0f;
    float tMax = FLT_MAX;

    for (int i = 0; i < 3; ++i)
    {
        float rayOrig = ((float*)&rayOrigin)[i];
        float rayDirI = ((float*)&rayDir)[i];
        float bMin = ((float*)&boxMin)[i];
        float bMax = ((float*)&boxMax)[i];

        if (fabsf(rayDirI) < EPSILON)
        {
            if (rayOrig < bMin || rayOrig > bMax)
            {
                return false;
            }
        }
        else
        {
            float invDir = 1.0f / rayDirI;
            float t1 = (bMin - rayOrig) * invDir;
            float t2 = (bMax - rayOrig) * invDir;

            if (t1 > t2)
            {
                std::swap(t1, t2);
            }

            tMin = max(tMin, t1);
            tMax = min(tMax, t2);

            if (tMin > tMax)
            {
                return false;
            }
        }
    }

    outDist = tMin;
    return true;
}
//=============================================================================
// ���C��OBB�̓����蔻��
//=============================================================================
bool CRayCast::IntersectOBB(
    const D3DXVECTOR3& rayOrigin,
    const D3DXVECTOR3& rayDir,
    const D3DXMATRIX& worldMatrix,
    const D3DXVECTOR3& halfSize,
    float& outDistance
)
{
    // ���[���h�����[�J���ւ̋t�s����v�Z
    D3DXMATRIX invWorld;
    if (D3DXMatrixInverse(&invWorld, NULL, &worldMatrix) == NULL)
    {
        return false;
    }

    // ���C�����[�J����Ԃɕϊ�
    D3DXVECTOR3 localOrigin, localDir;
    D3DXVec3TransformCoord(&localOrigin, &rayOrigin, &invWorld);
    D3DXVec3TransformNormal(&localDir, &rayDir, &invWorld);
    D3DXVec3Normalize(&localDir, &localDir);

    // ���[�J����Ԃ�AABB�ɑ΂��郌�C�����蔻��
    // AABB��min/max���v�Z
    D3DXVECTOR3 boxMin = -halfSize;
    D3DXVECTOR3 boxMax = +halfSize;

    float tmin = -FLT_MAX;
    float tmax = FLT_MAX;

    // �e�����ƂɃX���u�@�iSlab method�j�Ō����v�Z
    for (int i = 0; i < 3; i++)
    {
        float origin = ((float*)&localOrigin)[i];
        float dir = ((float*)&localDir)[i];
        float min = ((float*)&boxMin)[i];
        float max = ((float*)&boxMax)[i];

        if (fabsf(dir) < 1e-6f)
        {
            if (origin < min || origin > max)
            {
                return false;
            }
        }
        else
        {
            float t1 = (min - origin) / dir;
            float t2 = (max - origin) / dir;

            if (t1 > t2)
            {
                std::swap(t1, t2);
            }
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);

            if (tmin > tmax)
            {
                return false;
            }
        }
    }

    // �ŏ�����������Ԃ�

    if (tmin < 0.0f)
    {
        return false; // ���C�̎n�_�����̌����͖���
    }

    outDistance = tmin;
    return true;
}
