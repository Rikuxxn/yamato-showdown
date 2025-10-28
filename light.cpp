//=============================================================================
//
// ���C�g���� [light.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "light.h"
#include "renderer.h"
#include "manager.h"
#include "objectBillboard.h"

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
int CLight::m_lightCount = 0;
CLight::LightInfo CLight::m_lights[MAX_LIGHTS] = {};
CObjectX* CLight::m_pObjectX = nullptr;

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CLight::CLight()
{
	// �l�̃N���A
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CLight::~CLight()
{
	// ���C�g�̏I������
	Uninit();
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CLight::Init(void)
{
	m_lightCount = 0;

	for (int nCnt = 0; nCnt < MAX_LIGHTS; nCnt++)
	{
		m_lights[nCnt].enabled = false;
	}

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CLight::Uninit(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	for (int nCnt = 0; nCnt < MAX_LIGHTS; nCnt++)
	{
		if (m_lights[nCnt].enabled)
		{
			pDevice->LightEnable(nCnt, FALSE);
			m_lights[nCnt].enabled = false;
		}
	}

	m_lightCount = 0;
}
//=============================================================================
// �X�V����
//=============================================================================
void CLight::Update(void)
{
    // �Ȃ�
}
//=============================================================================
// ���C�g�̒ǉ�����
//=============================================================================
int CLight::AddLight(D3DLIGHTTYPE type, const D3DXCOLOR& diffuse, const D3DXVECTOR3& direction, const D3DXVECTOR3& position)
{
    if (m_lightCount >= MAX_LIGHTS)
    {
        return -1;
    }

    int index = m_lightCount;
    LightInfo& lightInfo = m_lights[index];

    ZeroMemory(&lightInfo.light, sizeof(D3DLIGHT9));
    lightInfo.light.Type = type;
    lightInfo.light.Diffuse = diffuse;
    lightInfo.position = position;
    lightInfo.light.Position = position;

    D3DXVECTOR3 dir = direction;
    if (D3DXVec3Length(&dir) == 0.0f)
    {
        dir = D3DXVECTOR3(0, -1, 0); // �f�t�H���g������
    }

    D3DXVec3Normalize(&lightInfo.direction, &dir);
    lightInfo.light.Direction = lightInfo.direction;

//#ifdef _DEBUG
//
//    // ���C�g�p���f���̐���
//    m_pObjectX = CObjectX::Create("data/MODELS/light.x", position, lightInfo.light.Direction, D3DXVECTOR3(1.0f, 1.0f, 1.0f));
//
//#endif // DEBUG

    // ���C�g�̎�ނɉ������ݒ�
    if (type == D3DLIGHT_POINT)
    {
        lightInfo.light.Attenuation0 = 0.0f;
        lightInfo.light.Attenuation1 = 0.02f;
        lightInfo.light.Attenuation2 = 0.0f;
        lightInfo.light.Range = 105.0f;
    }
    else if (type == D3DLIGHT_SPOT)
    {
        lightInfo.light.Range = 1000.0f;
        lightInfo.light.Theta = D3DXToRadian(70.0f);  // ���邭�Ƃ炷�͈́i���S�j
        lightInfo.light.Phi = D3DXToRadian(95.0f);  // �������Ă����O��        
        lightInfo.light.Falloff = 1.0f;
        lightInfo.light.Attenuation0 = 1.0f;
        lightInfo.light.Attenuation1 = 0.05f;    // �ɂ₩�Ȍ���
        lightInfo.light.Attenuation2 = 0.0f;
    }

    // �f�o�C�X�̎擾
    LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

    pDevice->SetLight(index, &lightInfo.light);
    pDevice->LightEnable(index, TRUE);

    lightInfo.enabled = true;

    // ���C�g�̃J�E���g�𑝂₷
    m_lightCount++;

    return index;
}
//=============================================================================
// ���C�g�̍폜����
//=============================================================================
void CLight::DeleteLight(int index)
{
    if (index >= 0 && index < MAX_LIGHTS && m_lights[index].enabled)
    {
        // �f�o�C�X�̎擾
        LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

        // ���C�g���I�t�ɂ���
        pDevice->LightEnable(index, FALSE);
        m_lights[index].enabled = false;

        // ���C�g�̃J�E���g�����炷
        m_lightCount--;
    }
}
//=============================================================================
// ���݂̃��C�g���擾���鏈��
//=============================================================================
std::vector<CLight::LightInfo> CLight::GetCurrentLights(void)
{
    std::vector<LightInfo> backup;

    for (int i = 0; i < m_lightCount; i++)
    {
        if (m_lights[i].enabled)
        {
            LightInfo b;
            b.light = m_lights[i].light;
            b.direction = m_lights[i].direction;
            b.position = m_lights[i].position;
            b.enabled = m_lights[i].enabled;
            backup.push_back(b);
        }
    }
    return backup;
}
//=============================================================================
// �ꎞ�I�Ƀ��C�g��ޔ����鏈��
//=============================================================================
void CLight::RestoreLights(const std::vector<LightInfo>& backup)
{
    Uninit(); // ��x�S���폜���Ă���

    for (const auto& b : backup)
    {
        int index = AddLight(b.light.Type,
            D3DXCOLOR(b.light.Diffuse),
            b.direction,
            b.position);
        // ����D3DLIGHT9�\���̂��㏑�����Ċ��S����
        m_lights[index].light = b.light;
        m_lights[index].enabled = b.enabled;
    }
}
