//=============================================================================
//
// ���b�V���V�����_�[���� [meshcylinder.h]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "meshcylinder.h"
#include "manager.h"

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CMeshCylinder::CMeshCylinder(int nPriority) : CObject(nPriority)
{
	// �l�̃N���A
	m_pVtxBuff		= nullptr;	// ���_�o�b�t�@�ւ̃|�C���^
	m_pIdxBuff		= nullptr;	// �C���f�b�N�X�o�b�t�@�ւ̃|�C���^
	m_mtxWorld		= {};		// ���[���h�}�g���b�N�X
	m_pos			= INIT_VEC3;// �ʒu
	m_rot			= INIT_VEC3;// ����
	m_col			= INIT_XCOL;// �F
	m_fRadius		= 0.0f;		// ���a
	m_incRadius		= 0.0f;		// ���a�̑�����
	m_nLife			= 0;		// ���݂̎���
	m_nMaxLife		= 0;		// �ݒ莞�̎���
	m_decAlpha		= 0.0f;		// �A���t�@�l�̌�����
	m_fLineweight	= 0.0f;		// ����
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CMeshCylinder::~CMeshCylinder()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CMeshCylinder* CMeshCylinder::Create(D3DXVECTOR3 pos, D3DXCOLOR col, float fRad, float fWeight, float incRad, int nLife, float decAlpha)
{
	CMeshCylinder* pCylinder = new CMeshCylinder;

	pCylinder->SetPos(pos);
	pCylinder->SetCol(col);
	pCylinder->SetRadius(fRad);
	pCylinder->SetLineweight(fWeight);
	pCylinder->SetIncRadius(incRad);
	pCylinder->SetLife(nLife);
	pCylinder->SetDecAlpha(decAlpha);

	// ����������
	pCylinder->Init();

	return pCylinder;
}
//=============================================================================
// ����������
//=============================================================================
HRESULT CMeshCylinder::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// ���_�o�b�t�@�̐���
	pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * MESHCYLINDER_VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&m_pVtxBuff,
		NULL);

	// �C���f�b�N�X�o�b�t�@�̐���
	pDevice->CreateIndexBuffer(sizeof(WORD) * MESHCYLINDER_INDEX,
		D3DUSAGE_WRITEONLY,
		D3DFMT_INDEX16,
		D3DPOOL_MANAGED,
		&m_pIdxBuff,
		NULL);

	VERTEX_3D* pVtx = nullptr;// ���_���ւ̃|�C���^

	int nCnt = 0;
	//float tex = 1.0f / MESHCYLINDER_X;
	//float tex2 = 1.0f / MESHCYLINDER_Z;
	D3DXVECTOR3 nor;

	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntV = 0; nCntV < MESHCYLINDER_V + 1; nCntV++)
	{
		for (int nCntH = 0; nCntH < MESHCYLINDER_H + 1; nCntH++)
		{
			float Angle = (D3DX_PI * 2.0f) / MESHCYLINDER_H * nCntH;

			pVtx[nCnt].pos = D3DXVECTOR3(
				sinf(Angle) * m_fRadius - sinf(Angle),
				0.0f, 
				cosf(Angle) * m_fRadius - cosf(Angle)
			);

			nor.x = pVtx[nCnt].pos.x - 0.0f;
			nor.y = pVtx[nCnt].pos.y - 0.0f;
			nor.z = pVtx[nCnt].pos.z - 0.0f;

			// �@���̐��K��
			D3DXVec3Normalize(&pVtx[nCnt].nor, &nor);

			// ���_�J���[�̐ݒ�
			pVtx[nCnt].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			// �e�N�X�`�����W�̐ݒ�
			pVtx[nCnt].tex = D3DXVECTOR2(/*tex * */(float)nCntV, /*tex2 * */(float)nCntH);

			nCnt++;
		}
	}

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();

	WORD* pIdx;// �C���f�b�N�X���ւ̃|�C���^

	// �C���f�b�N�X�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pIdxBuff->Lock(0, 0, (void**)&pIdx, 0);

	int nCntIdx3 = MESHCYLINDER_X + 1;
	int Num = 0;
	int nCntNum = 0;

	// �C���f�b�N�X�̐ݒ�
	for (int nCntIdx = 0; nCntIdx < MESHCYLINDER_Z; nCntIdx++)
	{
		for (int nCntIdx2 = 0; nCntIdx2 <= MESHCYLINDER_X; nCntIdx2++, nCntIdx3++, Num++)
		{
			pIdx[nCntNum] = (WORD)nCntIdx3;

			pIdx[nCntNum + 1] = (WORD)Num;

			nCntNum += 2;
		}

		// �Ō�̍s���ǂ���
		// NOTE: �Ō�̍s�ɏk�ރ|���S��������
		if (nCntIdx != MESHCYLINDER_Z - 1)
		{
			pIdx[nCntNum] = (WORD)Num - 1;

			pIdx[nCntNum + 1] = (WORD)nCntIdx3;

			nCntNum += 2;
		}
	}

	// �C���f�b�N�X�o�b�t�@���A�����b�N����
	m_pIdxBuff->Unlock();

	return S_OK;
}
//=============================================================================
// �I������
//=============================================================================
void CMeshCylinder::Uninit(void)
{
	// �C���f�b�N�X�o�b�t�@�̉��
	if (m_pIdxBuff != nullptr)
	{
		m_pIdxBuff->Release();
		m_pIdxBuff = nullptr;
	}

	// ���_�o�b�t�@�̔j��
	if (m_pVtxBuff != nullptr)
	{
		m_pVtxBuff->Release();
		m_pVtxBuff = nullptr;
	}

	// �I�u�W�F�N�g�̔j��(�������g)
	this->Release();
}
//=============================================================================
// �X�V����
//=============================================================================
void CMeshCylinder::Update(void)
{
	m_col.a -= m_decAlpha;// �A���t�@�l�����X�ɉ�����

	m_nLife--;

	float fMugLife = (float)m_nLife / 50;
	float fAlpha = m_col.a * fMugLife;

	// �����܂��̓A���t�@�l��0�ɂȂ�����
	if (m_nLife <= 0)
	{
		// �I��
		Uninit();
		return;
	}

	// ���a�����X�ɍL����
	m_fRadius += m_incRadius;

	VERTEX_3D* pVtx = nullptr;// ���_���ւ̃|�C���^

	int nCnt = 0;
	
	// ���_�o�b�t�@�����b�N���A���_���ւ̃|�C���^���擾
	m_pVtxBuff->Lock(0, 0, (void**)&pVtx, 0);

	for (int nCntV = 0; nCntV < MESHCYLINDER_V + 1; nCntV++)
	{
		for (int nCntH = 0; nCntH < MESHCYLINDER_H + 1; nCntH++)
		{
			float Angle = (D3DX_PI * 2.0f) / MESHCYLINDER_H * nCntH;

			pVtx[nCnt].pos = D3DXVECTOR3(
				sinf(Angle) * m_fRadius - sinf(Angle) * nCntV * m_fLineweight,
				0.0f,
				cosf(Angle) * m_fRadius - cosf(Angle) * nCntV * m_fLineweight
			);

			// ���_�J���[�̐ݒ�
			float alpha;
			alpha = fAlpha * (1.0f - nCntV);
			pVtx[nCnt].col = D3DXCOLOR(m_col.r,m_col.g,m_col.b, alpha);

			nCnt++;
		}
	}

	// ���_�o�b�t�@���A�����b�N����
	m_pVtxBuff->Unlock();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CMeshCylinder::Draw(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	// �J�����O�ݒ�𖳌���
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	// ���u�����f�B���O�����Z�����ɐݒ�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);

	// ���e�X�g��L��
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);//�f�t�H���g��false
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);//0���傫��������`��

	// �v�Z�p�}�g���b�N�X
	D3DXMATRIX mtxRot, mtxTrans;

	// ���[���h�}�g���b�N�X�̏�����
	D3DXMatrixIdentity(&m_mtxWorld);

	// �����𔽉f
	D3DXMatrixRotationYawPitchRoll(&mtxRot, m_rot.y, m_rot.x, m_rot.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxRot);

	// �ʒu�𔽉f
	D3DXMatrixTranslation(&mtxTrans, m_pos.x, m_pos.y, m_pos.z);
	D3DXMatrixMultiply(&m_mtxWorld, &m_mtxWorld, &mtxTrans);

	// ���[���h�}�g���b�N�X��ݒ�
	pDevice->SetTransform(D3DTS_WORLD, &m_mtxWorld);

	// ���_�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetStreamSource(0, m_pVtxBuff, 0, sizeof(VERTEX_3D));

	// �C���f�b�N�X�o�b�t�@���f�[�^�X�g���[���ɐݒ�
	pDevice->SetIndices(m_pIdxBuff);

	// ���_�t�H�[�}�b�g�̐ݒ�
	pDevice->SetFVF(FVF_VERTEX_3D);

	// �e�N�X�`���̐ݒ�
	pDevice->SetTexture(0, nullptr);

	// �|���S���̕`��
	pDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, 0, MESHCYLINDER_VERTEX, 0, MESHCYLINDER_PRIMITIVE);

	// ���e�X�g�𖳌��ɖ߂�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//�f�t�H���g��false

	// �J�����O�ݒ��L����
	pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

	// ���e�X�g�𖳌��ɖ߂�
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);//�f�t�H���g��false

	// ���u�����f�B���O�����ɖ߂�
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
}
