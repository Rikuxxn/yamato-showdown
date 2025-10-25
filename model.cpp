//=============================================================================
//
// ���f������ [model.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "model.h"
#include "texture.h"
#include "renderer.h"
#include "manager.h"
#include <cstdio>

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CModel::CModel()
{
	// �l�̃N���A
	memset(m_Path, 0, sizeof(m_Path));	// �t�@�C���p�X
	m_nIdxTexture	= 0;				// �e�N�X�`���C���f�b�N�X
	m_pos			= INIT_VEC3;		// �ʒu
	m_rot			= INIT_VEC3;		// ����
	m_move			= INIT_VEC3;		// �ړ���
	m_pMesh			= nullptr;			// ���b�V���ւ̃|�C���^
	m_pBuffMat		= nullptr;			// �}�e���A���ւ̃|�C���^
	m_dwNumMat		= NULL;				// �}�e���A����
	m_mtxWorld		= {};				// ���[���h�}�g���b�N�X
	m_pParent		= nullptr;			// �e���f���ւ̃|�C���^

	m_pOutlineVS	= nullptr;
	m_pOutlinePS	= nullptr;
	m_pVSConsts		= nullptr;
	m_pPSConsts		= nullptr;
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CModel::~CModel()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CModel* CModel::Create(const char* pFilepath, D3DXVECTOR3 pos, D3DXVECTOR3 rot)
{
	CModel* pModel = new CModel;

	pModel->m_pos = pos;
	pModel->m_rot = D3DXToRadian(rot);
	strcpy_s(pModel->m_Path, pFilepath); // �p�X�ۑ�

	// ����������
	pModel->Init();

	return pModel;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CModel::Init(void)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// X�t�@�C���̓ǂݍ���
	D3DXLoadMeshFromX(m_Path,
		D3DXMESH_SYSTEMMEM,
		pDevice,
		NULL,
		&m_pBuffMat,
		NULL,
		&m_dwNumMat,
		&m_pMesh);

	int nNumVtx;		// ���_��
	DWORD sizeFVF;		// ���_�t�H�[�}�b�g�̃T�C�Y
	BYTE* pVtxBuff;		// ���_�o�b�t�@�ւ̃|�C���^

	// ���b�V�����ǂݍ��܂�Ă��邩�m�F
	if (m_pMesh == nullptr)
	{
		MessageBox(nullptr, "X�t�@�C���̓ǂݍ��݂Ɏ��s���܂����i���b�V����NULL�ł��j", "�G���[", MB_OK | MB_ICONERROR);
		return 0;
	}

	// �X���[�Y��
	{
		ID3DXMesh* pTempMesh = nullptr;

		// �אڏ����쐬
		DWORD* pAdjacency = new DWORD[m_pMesh->GetNumFaces() * 3];
		m_pMesh->GenerateAdjacency(1e-6f, pAdjacency);

		// �@�������i�X���[�Y���j
		HRESULT hr = D3DXComputeNormals(m_pMesh, pAdjacency);

		if (FAILED(hr))
		{
			// ���f���ɖ@���������ꍇ�A��������N���[�����Ė@����t�^
			D3DVERTEXELEMENT9 decl[MAX_FVF_DECL_SIZE];
			m_pMesh->GetDeclaration(decl);
			m_pMesh->CloneMesh(D3DXMESH_SYSTEMMEM, decl, pDevice, &pTempMesh);

			if (pTempMesh)
			{
				pTempMesh->GenerateAdjacency(1e-6f, pAdjacency);
				D3DXComputeNormals(pTempMesh, pAdjacency);
				m_pMesh->Release();
				m_pMesh = pTempMesh;
			}
		}

		delete[] pAdjacency;
	}

	// ���_���̎擾
	nNumVtx = m_pMesh->GetNumVertices();

	// ���_�t�H�[�}�b�g�̎擾
	sizeFVF = D3DXGetFVFVertexSize(m_pMesh->GetFVF());

	// ���_�o�b�t�@�̃��b�N
	m_pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&pVtxBuff);


	// �ő�l�E�ŏ��l�͂Ȃ�


	// ���_�o�b�t�@�̃A�����b�N
	m_pMesh->UnlockVertexBuffer();

	D3DXMATERIAL* pMat;// �}�e���A���ւ̃|�C���^

	// �}�e���A���f�[�^�ւ̃|�C���^���擾
	pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	m_nIdxTexture = new int[m_dwNumMat];

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		if (pMat[nCntMat].pTextureFilename != nullptr)
		{// �e�N�X�`���t�@�C�������݂���
			// �e�N�X�`���̓o�^
			m_nIdxTexture[nCntMat] = pTexture->RegisterDynamic(pMat[nCntMat].pTextureFilename);
		}
		else
		{// �e�N�X�`�������݂��Ȃ�
			m_nIdxTexture[nCntMat] = -1;
		}
	}

	// �����_���[�̎擾
	CRenderer* pRenderer = CManager::GetRenderer();

	// ���_�V�F�[�_���R���p�C��
	pRenderer->CompileVertexShader("OutlineVS.hlsl", "VSMain", &m_pOutlineVS, &m_pVSConsts);

	// �s�N�Z���V�F�[�_���R���p�C��
	pRenderer->CompilePixelShader("OutlinePS.hlsl", "PSMain", &m_pOutlinePS, &m_pPSConsts);

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CModel::Uninit(void)
{
	// �e�N�X�`���C���f�b�N�X�̔j��
	if (m_nIdxTexture != nullptr)
	{
		delete[] m_nIdxTexture;
		m_nIdxTexture = nullptr;
	}

	// ���b�V���̔j��
	if (m_pMesh != nullptr)
	{
		m_pMesh->Release();
		m_pMesh = nullptr;
	}

	// �}�e���A���̔j��
	if (m_pBuffMat != nullptr)
	{
		m_pBuffMat->Release();
		m_pBuffMat = nullptr;
	}

	// �V�F�[�_�[�̔j��
	if (m_pOutlineVS) { m_pOutlineVS->Release(); m_pOutlineVS = nullptr; }
	if (m_pOutlinePS) { m_pOutlinePS->Release(); m_pOutlinePS = nullptr; }
	if (m_pVSConsts) { m_pVSConsts->Release();  m_pVSConsts = nullptr; }
	if (m_pPSConsts) { m_pPSConsts->Release();  m_pPSConsts = nullptr; }
}
//=============================================================================
// �X�V����
//=============================================================================
void CModel::Update(void)
{
	// �ʒu���X�V
	m_pos.x += m_move.x;
	m_pos.z += m_move.z;
	m_pos.y += m_move.y;
}
//=============================================================================
// �`�揈��
//=============================================================================
void CModel::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �������f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y + m_OffsetRot.y, m_rot.x + m_OffsetRot.x, m_rot.z + m_OffsetRot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu���f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x + m_OffsetPos.x, m_pos.y + m_OffsetPos.y, m_pos.z + m_OffsetPos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	D3DXMATRIX mtxParent;

	if (m_pParent != nullptr)
	{// �e�����݂���
		// �e���f���̃}�g���b�N�X���擾
		mtxParent = m_pParent->GetMtxWorld();
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);
	}
	else
	{// �e�����݂��Ȃ�
		// ����(�ŐV)�̃}�g���b�N�X���擾����
		pDevice->GetTransform(D3DTS_WORLD, &mtxParent);
		D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxParent);
	}

	// ���[���h�}�g���b�N�X�ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ===== �A�E�g���C���`�� =====
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW); // �J�����O���]
	pDevice->SetVertexShader(m_pOutlineVS);
	pDevice->SetPixelShader(m_pOutlinePS);

	// �萔�̐ݒ菈��
	SetOutlineShaderConstants(pDevice);

	for (int n = 0; n < (int)m_dwNumMat; n++)
	{
		// ���f���̕`��(�A�E�g���C���p)
		m_pMesh->DrawSubset(n);
	}

	// ��ԃ��Z�b�g
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
	pDevice->SetVertexShader(nullptr);
	pDevice->SetPixelShader(nullptr);

	// ===== �ʏ�`�� =====
	DrawNormal(pDevice);
}
//=============================================================================
// �ʏ탂�f���`�揈��
//=============================================================================
void CModel::DrawNormal(LPDIRECT3DDEVICE9 pDevice)
{
	// �e�N�X�`���̎擾
	CTexture* pTexture = CManager::GetTexture();

	D3DMATERIAL9 matDef;

	// ���݂̃}�e���A���̕ۑ�
	pDevice->GetMaterial(&matDef);

	// �}�e���A���f�[�^�ւ̃|�C���^
	D3DXMATERIAL* pMat = (D3DXMATERIAL*)m_pBuffMat->GetBufferPointer();

	for (int nCntMat = 0; nCntMat < (int)m_dwNumMat; nCntMat++)
	{
		// �}�e���A���ݒ�
		pDevice->SetMaterial(&pMat[nCntMat].MatD3D);

		// �e�N�X�`���ݒ�
		if (m_nIdxTexture[nCntMat] == -1)
			pDevice->SetTexture(0, nullptr);
		else
			pDevice->SetTexture(0, pTexture->GetAddress(m_nIdxTexture[nCntMat]));

		// �`��
		m_pMesh->DrawSubset(nCntMat);
	}

	// ���}�e���A���ɖ߂�
	pDevice->SetMaterial(&matDef);
}
//=============================================================================
// �萔�ݒ菈��
//=============================================================================
void CModel::SetOutlineShaderConstants(LPDIRECT3DDEVICE9 pDevice)
{
	// �s����擾
	D3DXMATRIX view, proj, wvp;
	pDevice->GetTransform(D3DTS_WORLD, &m_mtxWorld);
	pDevice->GetTransform(D3DTS_VIEW, &view);
	pDevice->GetTransform(D3DTS_PROJECTION, &proj);

	wvp = m_mtxWorld * view * proj;

	// �萔���V�F�[�_�ɓn��
	if (m_pVSConsts) // ���_�V�F�[�_�萔�e�[�u��
	{
		m_pVSConsts->SetMatrix(pDevice, "g_mWorld", &m_mtxWorld);
		m_pVSConsts->SetMatrix(pDevice, "g_mWorldViewProj", &wvp);
		m_pVSConsts->SetFloat(pDevice, "g_OutlineWidth", 0.2f); // �A�E�g���C������
	}

	if (m_pPSConsts) // �s�N�Z���V�F�[�_�萔�e�[�u��
	{
		D3DXVECTOR4 col(0, 0, 0, 1); // ���A�E�g���C��
		m_pPSConsts->SetVector(pDevice, "g_OutlineColor", &col);
	}
}