//=============================================================================
//
// �u���b�N�}�l�[�W���[���� [blockmanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "blockmanager.h"
#include "json.hpp"
#include "FileDialogUtils.h"
#include "manager.h"
#include "imgui_internal.h"
#include "raycast.h"
#include "game.h"
#include "blocklist.h"
#include "guiInfo.h"

// JSON�̎g�p
using json = nlohmann::json;

//*****************************************************************************
// �ÓI�����o�ϐ��錾
//*****************************************************************************
std::vector<CBlock*> CBlockManager::m_blocks = {};	// �u���b�N�̏��
std::unordered_map<CBlock::TYPE, std::vector<CBlock*>> CBlockManager::m_blocksByType;
int CBlockManager::m_selectedIdx = 0;				// �I�𒆂̃C���f�b�N�X
CBlock* CBlockManager::m_draggingBlock = {};		// �h���b�O���̃u���b�N
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_FilePathMap; 
CBlock* CBlockManager::m_selectedBlock = {};// �I�������u���b�N

//=============================================================================
// �R���X�g���N�^
//=============================================================================
CBlockManager::CBlockManager()
{
	// �l�̃N���A
	m_selectedBlock				= nullptr;
	m_prevSelectedIdx			= -1;
	m_hasConsumedPayload		= false;
	m_pDebug3D					= nullptr;// 3D�f�o�b�O�\���ւ̃|�C���^
	m_autoUpdateColliderSize	= true;
	m_isDragging				= false;// �h���b�O�����ǂ���
}
//=============================================================================
// �f�X�g���N�^
//=============================================================================
CBlockManager::~CBlockManager()
{
	// �Ȃ�
}
//=============================================================================
// ��������
//=============================================================================
CBlock* CBlockManager::CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos)
{
	const char* path = CBlockManager::GetFilePathFromType(type);

	CBlock* newBlock = CBlock::Create(path, pos, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), type);

	if (newBlock)
	{
		// �S�̂̃��X�g�ɒǉ�
		m_blocks.push_back(newBlock);

		// �^�C�v�ʃL���b�V���ɂ��ǉ�
		m_blocksByType[type].push_back(newBlock);
	}

	return newBlock;
}
//=============================================================================
// ����������
//=============================================================================
void CBlockManager::Init(void)
{
	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	InitThumbnailRenderTarget(pDevice);

	LoadConfig("data/block_payload.json");

	// �T���l�C���L���b�V���쐬
	GenerateThumbnailsForResources();

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();
}
//=============================================================================
// �T���l�C���̃����_�[�^�[�Q�b�g�̏�����
//=============================================================================
HRESULT CBlockManager::InitThumbnailRenderTarget(LPDIRECT3DDEVICE9 device)
{
	if (!device)
	{
		return E_FAIL;
	}

	// �T���l�C���p�����_�[�^�[�Q�b�g�e�N�X�`���̍쐬
	if (FAILED(device->CreateTexture(
		(UINT)m_thumbWidth, (UINT)m_thumbHeight,
		1, D3DUSAGE_RENDERTARGET,
		D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT,
		&m_pThumbnailRT, nullptr)))
	{
		return E_FAIL;
	}

	// �T���l�C���p�[�x�X�e���V���T�[�t�F�X�̍쐬
	if (FAILED(device->CreateDepthStencilSurface(
		(UINT)m_thumbWidth, (UINT)m_thumbHeight,
		D3DFMT_D24S8,
		D3DMULTISAMPLE_NONE, 0, TRUE,
		&m_pThumbnailZ, nullptr)))
	{
		return E_FAIL;
	}

	return S_OK;
}
//=============================================================================
// �T���l�C���̃����_�[�^�[�Q�b�g�̐ݒ�
//=============================================================================
IDirect3DTexture9* CBlockManager::RenderThumbnail(CBlock* pBlock)
{
	if (!pBlock || !m_pThumbnailRT || !m_pThumbnailZ)
	{
		return nullptr;
	}

	// �f�o�C�X�̎擾
	LPDIRECT3DDEVICE9 pDevice = CManager::GetRenderer()->GetDevice();

	if (!pDevice)
	{
		return nullptr;
	}

	// �T���l�C���`��p�̐V�K�e�N�X�`���쐬
	IDirect3DTexture9* pTex = nullptr;
	if (FAILED(pDevice->CreateTexture(
		(UINT)m_thumbWidth, (UINT)m_thumbHeight, 1,
		D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8,
		D3DPOOL_DEFAULT, &pTex, nullptr)))
	{
		return nullptr;
	}

	// ���݂̃����_�[�^�[�Q�b�g�Ɛ[�x�o�b�t�@��ۑ�
	LPDIRECT3DSURFACE9 pOldRT = nullptr;
	LPDIRECT3DSURFACE9 pOldZ = nullptr;
	LPDIRECT3DSURFACE9 pNewRT = nullptr;

	pDevice->GetRenderTarget(0, &pOldRT);
	pDevice->GetDepthStencilSurface(&pOldZ);
	pTex->GetSurfaceLevel(0, &pNewRT);

	// �T���l�C���p�̃����_�[�^�[�Q�b�g�ɐ؂�ւ�
	pDevice->SetRenderTarget(0, pNewRT);
	pDevice->SetDepthStencilSurface(m_pThumbnailZ);

	// �N���A
	pDevice->Clear(0, nullptr, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(50, 50, 50), 1.0f, 0);
	pDevice->BeginScene();

	// �Œ�J����
	D3DXVECTOR3 eye(-120.0f, 100.0f, -120.0f), at(0.0f, 0.0f, 0.0f), up(0.0f, 1.0f, 0.0f);
	D3DXMATRIX matView, matProj;
	D3DXMatrixLookAtLH(&matView, &eye, &at, &up);

	// �v���W�F�N�V�����}�g���b�N�X�̍쐬
	D3DXMatrixPerspectiveFovLH(&matProj,
		D3DXToRadian(60.0f),						// ����p
		m_thumbWidth / m_thumbHeight, // �A�X�y�N�g��
		1.0f,										// �߃N���b�v��
		1000.0f);									// ���N���b�v��

	pDevice->SetTransform(D3DTS_VIEW, &matView);
	pDevice->SetTransform(D3DTS_PROJECTION, &matProj);

	// --- ���C�g�̃o�b�N�A�b�v ---
	auto backup = CLight::GetCurrentLights();

	// �����̃��C�g�𖳌���
	CLight::Uninit();

	// ���C�g�̏����ݒ菈��
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.9f, 0.9f, 0.9f, 1.0f), D3DXVECTOR3(0.0f, -1.0f, 0.0f), D3DXVECTOR3(0.0f, 300.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.3f, 0.3f, 0.3f, 1.0f), D3DXVECTOR3(0.0f, 1.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(-1.0f, 0.0f, 0.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));
	CLight::AddLight(D3DLIGHT_DIRECTIONAL, D3DXCOLOR(0.7f, 0.7f, 0.7f, 1.0f), D3DXVECTOR3(0.0f, 0.0f, -1.0f), D3DXVECTOR3(0.0f, 0.0f, 0.0f));

	// �T���l�C���p���f���`��
	pBlock->Draw();

	// --- ���̃��C�g�𕜌� ---
	CLight::RestoreLights(backup);

	// �`��I��
	pDevice->EndScene();

	// ���̃����_�[�^�[�Q�b�g�Ɛ[�x�o�b�t�@�ɖ߂�
	pDevice->SetRenderTarget(0, pOldRT);
	pDevice->SetDepthStencilSurface(pOldZ);

	if (pOldRT) pOldRT->Release();
	if (pOldZ)  pOldZ->Release();
	if (pNewRT) pNewRT->Release();

	return pTex;
}
//=============================================================================
// �T���l�C���p�̃��f����������
//=============================================================================
void CBlockManager::GenerateThumbnailsForResources(void)
{
	// �����T���l�C�������
	for (auto tex : m_thumbnailTextures)
	{
		if (tex)
		{
			tex->Release();
		}
	}

	m_thumbnailTextures.clear();
	m_thumbnailTextures.resize((size_t)CBlock::TYPE_MAX, nullptr);

	for (size_t i = 0; i < (int)CBlock::TYPE_MAX; ++i)
	{
		// �ꎞ�u���b�N�����i�ʒu�͌��_�j
		CBlock::TYPE payloadType = static_cast<CBlock::TYPE>(i);
		CBlock* pTemp = CreateBlock(payloadType, D3DXVECTOR3(0, 0, 0));
		if (!pTemp)
		{
			continue;
		}

		// �T���l�C���쐬
		m_thumbnailTextures[i] = RenderThumbnail(pTemp);

		pTemp->Kill();                 // �폜�t���O�𗧂Ă�
		CleanupDeadBlocks();           // �z�񂩂��菜���A���������
	}
}
//=============================================================================
// �T���l�C���̔j��
//=============================================================================
void CBlockManager::ReleaseThumbnailRenderTarget(void)
{
	// �����_�[�^�[�Q�b�g�̔j��
	if (m_pThumbnailRT)
	{
		m_pThumbnailRT->Release();
		m_pThumbnailRT = nullptr;
	}

	if (m_pThumbnailZ)
	{
		m_pThumbnailZ->Release();
		m_pThumbnailZ = nullptr;
	}

	// �T���l�C���L���b�V����������Ă���
	for (auto& tex : m_thumbnailTextures)
	{
		if (tex)
		{
			tex->Release();
			tex = nullptr;
		}
	}
	m_thumbnailTextures.clear();
	m_thumbnailsGenerated = false;
}
//=============================================================================
// �I������
//=============================================================================
void CBlockManager::Uninit(void)
{
	// �T���l�C���̔j��
	ReleaseThumbnailRenderTarget();

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();
}
//=============================================================================
// �폜�\�񂪂���u���b�N�̍폜����
//=============================================================================
void CBlockManager::CleanupDeadBlocks(void)
{
	for (int i = (int)m_blocks.size() - 1; i >= 0; i--)
	{
		if (m_blocks[i]->IsDead())
		{
			// �u���b�N�̏I������
			m_blocks[i]->Uninit();
			m_blocks.erase(m_blocks.begin() + i);
		}
	}
}
//=============================================================================
// �X�V����
//=============================================================================
void CBlockManager::Update(void)
{
#ifdef _DEBUG

	// ���̍X�V
	UpdateInfo();

#endif
	// �u���b�N�폜����
	CleanupDeadBlocks();
}
//=============================================================================
// �`�揈��
//=============================================================================
void CBlockManager::Draw(void)
{
#ifdef _DEBUG
	//// �I�𒆂̃u���b�N�����R���C�_�[�`��
	//CBlock* pSelectBlock = GetSelectedBlock();
	//if (pSelectBlock != nullptr)
	//{
	//	pSelectBlock->DrawCollider();
	//}
#endif
}
//=============================================================================
// ���̍X�V����
//=============================================================================
void CBlockManager::UpdateInfo(void)
{
	// GUI�X�^�C���̎擾
	ImGuiStyle& style = ImGui::GetStyle();

	style.GrabRounding		= 10.0f;		// �X���C�_�[�̂܂݂��ۂ�
	style.ScrollbarRounding = 10.0f;		// �X�N���[���o�[�̊p
	style.ChildRounding		= 10.0f;		// �q�E�B���h�E�̊p
	style.WindowRounding	= 10.0f;		// �E�B���h�E�S�̂̊p

	// �ꏊ
	CImGuiManager::Instance().SetPosImgui(ImVec2(1900.0f, 20.0f));

	// �T�C�Y
	CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 500.0f));

	// �ŏ���GUI
	CImGuiManager::Instance().StartImgui(u8"BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// �u���b�N���Ȃ��ꍇ
	if (m_blocks.empty())
	{
		ImGui::Text("No blocks placed yet.");
	}
	else
	{
		// �u���b�N�̑���
		ImGui::Text("Block Num %d", (int)m_blocks.size());

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// �C���f�b�N�X�I��
		ImGui::SliderInt("Block Index", &m_selectedIdx, 0, (int)m_blocks.size() - 1);

		// �͈͊O�΍�
		if (m_selectedIdx >= (int)m_blocks.size())
		{
			m_selectedIdx = (int)m_blocks.size() - 1;
		}

		// �O��I��ł��u���b�N������
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != m_selectedIdx)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// �Ώۃu���b�N�̎擾
		m_selectedBlock = m_blocks[m_selectedIdx];

		// �u���b�N���̒�������
		UpdateTransform(m_selectedBlock);

		if (GetUpdateCollider() == false)
		{
			// �R���C�_�[�̒�������
			UpdateCollider(m_selectedBlock);
		}
		else
		{
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�
			ImGui::Text("Auto Update Collider Size is Active");
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	// �u���b�N�^�C�v�ꗗ
	if (ImGui::TreeNode("Block Types"))
	{
		ImGui::BeginChild("BlockTypeList", ImVec2(0, 500), true); // �X�N���[���̈�

		int numTypes = (int)CBlock::TYPE_MAX;

		for (int i = 0; i < numTypes; i++)
		{
			IDirect3DTexture9* pThumb = GetThumbnailTexture(i); // �T���l�C���擾

			if (!pThumb)
			{
				continue; // nullptr �̓X�L�b�v
			}

			ImGui::PushID(i);
			ImGui::Image(reinterpret_cast<ImTextureID>(pThumb), ImVec2(m_thumbWidth, m_thumbHeight));

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				m_isDragging = true;// �h���b�O��
				CBlock::TYPE payloadType = static_cast<CBlock::TYPE>(i);
				ImGui::SetDragDropPayload("BLOCK_TYPE", &payloadType, sizeof(payloadType));
				ImGui::Text("Block Type %d", i);
				ImGui::Image(reinterpret_cast<ImTextureID>(pThumb), ImVec2(m_thumbWidth, m_thumbHeight));
				ImGui::EndDragDropSource();
			}

			// �}�E�X�̎擾
			CInputMouse* pMouse = CManager::GetInputMouse();

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_isDragging)
			{
				// ���݂�ImGui�̓�����ԁi�R���e�L�X�g�j�ւ̃|�C���^�[���擾
				ImGuiContext* ctx = ImGui::GetCurrentContext();

				if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
				{
					// �h���b�O��������
					m_isDragging = false;

					CBlock::TYPE draggedType = *(CBlock::TYPE*)ctx->DragDropPayload.Data;

					D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
					pos.y = 30.0f;

					// �u���b�N�̐���
					m_draggingBlock = CreateBlock(draggedType, pos);
				}
			}

			ImGui::PopID();

			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �摜�Ԃ̗]��
		}

		ImGui::EndChild();
		ImGui::TreePop();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

	if (ImGui::Button("Save"))
	{
		// �_�C�A���O���J���ăt�@�C���ɕۑ�
		std::string path = OpenWindowsSaveFileDialog();

		if (!path.empty())
		{
			// �f�[�^�̕ۑ�
			CBlockManager::SaveToJson(path.c_str());
		}
	}

	ImGui::SameLine(0);

	if (ImGui::Button("Load"))
	{
		// �_�C�A���O���J���ăt�@�C�����J��
		std::string path = OpenWindowsOpenFileDialog();

		if (!path.empty())
		{
			// �f�[�^�̓ǂݍ���
			CBlockManager::LoadFromJson(path.c_str());
		}
	}

	ImGui::End();

	if (CManager::GetMode() == MODE_TITLE)
	{
		return;
	}

	// �}�E�X�I������
	PickBlockFromMouseClick();
}
//=============================================================================
// �u���b�N���̒�������
//=============================================================================
void CBlockManager::UpdateTransform(CBlock* selectedBlock)
{
	if (selectedBlock)
	{
		// �I�𒆂̃u���b�N�̐F��ς���
		selectedBlock->SetSelected(true);

		D3DXVECTOR3 pos = selectedBlock->GetPos();	// �I�𒆂̃u���b�N�̈ʒu�̎擾
		D3DXVECTOR3 rot = selectedBlock->GetRot();	// �I�𒆂̃u���b�N�̌����̎擾
		D3DXVECTOR3 size = selectedBlock->GetSize();// �I�𒆂̃u���b�N�̃T�C�Y�̎擾
		btScalar mass = selectedBlock->GetMass();	// �I�𒆂̃u���b�N�̎��ʂ̎擾

		// ���W�A�����p�x�Ɉꎞ�ϊ��i�ÓI�ϐ��ŕێ������쒆�̂ݍX�V�j
		static D3DXVECTOR3 degRot = D3DXToDegree(rot);
		static bool m_initializedDegRot = false;

		if (!m_initializedDegRot)
		{
			degRot = D3DXToDegree(rot);
			m_initializedDegRot = true;
		}

		bool isEditMode = selectedBlock->IsEditMode();

		ImGui::Checkbox("Kinematic", &isEditMode);

		if (isEditMode)
		{
			selectedBlock->SetEditMode(true);  // �`�F�b�N��Kinematic��
		}
		else
		{
			selectedBlock->SetEditMode(false); // �ʏ�ɖ߂�
		}

		//*********************************************************************
		// POS �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("POS"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_x", &pos.x, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_y", &pos.y, 1.0f, -9000.0f, 9000.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_pos_z", &pos.z, 1.0f, -9000.0f, 9000.0f, "%.1f");

		//*********************************************************************
		// ROT �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::Text("ROT"); ImGui::SameLine(60);

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedX = ImGui::DragFloat("##Block_rot_x", &degRot.x, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedY = ImGui::DragFloat("##Block_rot_y", &degRot.y, 0.1f, -180.0f, 180.0f, "%.2f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		bool changedZ = ImGui::DragFloat("##Block_rot_z", &degRot.z, 0.1f, -180.0f, 180.0f, "%.2f");

		bool isRotChanged = changedX || changedY || changedZ;
		
		//*********************************************************************
		// SIZE �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("SIZE"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_x", &size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_y", &size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Block_size_z", &size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		// �`�F�b�N�{�b�N�X�F�g�嗦�ɉ����Ď����X�V���邩
		ImGui::Checkbox("Auto Update Collider Size", &m_autoUpdateColliderSize);

		// �O��̃T�C�Y��ێ�
		static D3DXVECTOR3 prevSize = selectedBlock->GetSize();

		// �T�C�Y�ύX�`�F�b�N
		bool isSizeChanged = (size != prevSize);

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// �u���b�N�̓��ꏈ��
		selectedBlock->DrawCustomUI();

		//*********************************************************************
		// ���� �̒���
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("MASS"); ImGui::SameLine(60); // ���x���̈ʒu������ƒ���

		// �X���C�_�[�i�͈�: 0.0f �` 80.0f�j
		ImGui::SliderFloat("##MassSlider", &mass, 0.0f, 80.0f, "%.2f");


		// �p�x�����W�A���ɖ߂�
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		// �ʒu�̐ݒ�
		selectedBlock->SetPos(pos);

		// �T�C�Y�̐ݒ�
		selectedBlock->SetSize(size);

		// �T�C�Y(�g�嗦)���ς�����Ƃ������Ă�
		if (m_autoUpdateColliderSize == true && isSizeChanged)
		{
			// �u���b�N�T�C�Y�ɂ��R���C�_�[�̐���
			selectedBlock->CreatePhysicsFromScale(size);

			prevSize = size; // �X�V
		}

		if (isRotChanged)
		{
			// ��]���ς�����������Z�b�g
			selectedBlock->SetRot(rotRad);

			// �ҏW���[�h�Ȃ瑦����Transform���X�V���ē���
			if (isEditMode)
			{
				btTransform transform;
				transform.setIdentity();

				btVector3 btPos(pos.x + selectedBlock->GetColliderOffset().x,
					pos.y + selectedBlock->GetColliderOffset().y,
					pos.z + selectedBlock->GetColliderOffset().z);
				transform.setOrigin(btPos);

				D3DXMATRIX matRot;
				D3DXMatrixRotationYawPitchRoll(&matRot, rotRad.y, rotRad.x, rotRad.z);

				D3DXQUATERNION dq;
				D3DXQuaternionRotationMatrix(&dq, &matRot);

				btQuaternion btRot(dq.x, dq.y, dq.z, dq.w);
				transform.setRotation(btRot);

				if (!selectedBlock->GetRigidBody())
				{
					return;
				}
				selectedBlock->GetRigidBody()->setWorldTransform(transform);
				selectedBlock->GetRigidBody()->getMotionState()->setWorldTransform(transform);
			}
		}
		else
		{
			// �ҏW���Ă��Ȃ�����degRot��selectedBlock�̒l�ɓ������Ă���
			degRot = D3DXToDegree(selectedBlock->GetRot());
		}

		//*********************************************************************
		// �u���b�N�̍폜
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(1.0f, 0.3f, 0.3f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.6f, 0.0f, 0.0f, 1.0f));

		if (ImGui::Button("Delete"))
		{
			if (m_autoUpdateColliderSize)
			{
				if (m_blocks[m_selectedIdx])
				{
					// �I�𒆂̃u���b�N���폜
					m_blocks[m_selectedIdx]->Uninit();
				}

				m_blocks.erase(m_blocks.begin() + m_selectedIdx);

				// �I���C���f�b�N�X�𒲐�
				if (m_selectedIdx >= (int)m_blocks.size())
				{
					m_selectedIdx = (int)m_blocks.size() - 1;
				}

				m_prevSelectedIdx = -1;
			}
			else
			{// m_autoUpdateColliderSize��false�̎��͉������Ȃ�
				
			}
		}

		ImGui::PopStyleColor(3);
	}

	// �Ō�ɕۑ�
	m_prevSelectedIdx = m_selectedIdx;
}
//=============================================================================
// �u���b�N�I������
//=============================================================================
void CBlockManager::PickBlockFromMouseClick(void)
{
	// ImGui���}�E�X���g���Ă�Ȃ�I���������L�����Z��
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	// ���N���b�N�̂�
	if (!CManager::GetInputMouse()->GetTrigger(0))
	{
		return;
	}

	// ���C�擾�iCRayCast���g�p�j
	D3DXVECTOR3 rayOrigin, rayDir;
	CRayCast::GetMouseRay(rayOrigin, rayDir);

	float minDist = FLT_MAX;
	int hitIndex = -1;

	for (size_t i = 0; i < m_blocks.size(); ++i)
	{
		CBlock* block = m_blocks[i];

		// ���[���h�s��̎擾�i�ʒu�E��]�E�g����܂ށj
		D3DXMATRIX world = block->GetWorldMatrix();

		D3DXVECTOR3 modelSize = block->GetModelSize();
		D3DXVECTOR3 scale = block->GetSize();

		D3DXVECTOR3 halfSize;
		halfSize.x = modelSize.x * 0.5f;
		halfSize.y = modelSize.y * 0.5f;
		halfSize.z = modelSize.z * 0.5f;

		float dist = 0.0f;
		if (CRayCast::IntersectOBB(rayOrigin, rayDir, world, halfSize, dist))
		{
			if (dist < minDist)
			{
				minDist = dist;
				hitIndex = i;
			}
		}
	}

	// �I����Ԃ𔽉f
	if (hitIndex >= 0)
	{
		// �ȑO�I�΂�Ă����u���b�N���I����
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != hitIndex)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// �V�����I��
		m_selectedIdx = hitIndex;
		m_blocks[m_selectedIdx]->SetSelected(true);
		m_prevSelectedIdx = hitIndex;
	}
}
//=============================================================================
// �R���C�_�[�̒�������
//=============================================================================
void CBlockManager::UpdateCollider(CBlock* selectedBlock)
{
	// �P��R���C�_�[�p
	D3DXVECTOR3 colliderSize = selectedBlock->GetColliderSize();
	static D3DXVECTOR3 prevSize = colliderSize;

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("COLLIDER SIZE");

	ImGui::Dummy(ImVec2(0.0f, 10.0f));

	bool changed = false;

	ImGui::Text("X:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	changed |= ImGui::DragFloat("##collider_size_x", &colliderSize.x, 0.1f, 0.1f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Y:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	changed |= ImGui::DragFloat("##collider_size_y", &colliderSize.y, 0.1f, 0.1f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Z:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	changed |= ImGui::DragFloat("##collider_size_z", &colliderSize.z, 0.1f, 0.1f, 800.0f, "%.1f");

	if (changed && colliderSize != prevSize)
	{
		colliderSize.x = std::max(colliderSize.x, 0.01f);
		colliderSize.y = std::max(colliderSize.y, 0.01f);
		colliderSize.z = std::max(colliderSize.z, 0.01f);

		selectedBlock->SetColliderManual(colliderSize);

		prevSize = colliderSize;
	}

	D3DXVECTOR3 offset = selectedBlock->GetColliderOffset();
	static D3DXVECTOR3 prevOffset = offset;

	ImGui::Dummy(ImVec2(0.0f, 10.0f));
	ImGui::Text("COLLIDER OFFSET");

	bool offsetChanged = false;

	ImGui::Text("X:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	offsetChanged |= ImGui::DragFloat("##collider_offset_x", &offset.x, 0.1f, -800.0f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Y:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	offsetChanged |= ImGui::DragFloat("##collider_offset_y", &offset.y, 0.1f, -800.0f, 800.0f, "%.1f");

	ImGui::SameLine();
	ImGui::Text("Z:"); ImGui::SameLine();
	ImGui::SetNextItemWidth(80);
	offsetChanged |= ImGui::DragFloat("##collider_offset_z", &offset.z, 0.1f, -800.0f, 800.0f, "%.1f");

	if (offsetChanged && offset != prevOffset)
	{
		selectedBlock->SetColliderOffset(offset);
		selectedBlock->CreatePhysics(selectedBlock->GetPos(), selectedBlock->GetColliderSize());

		prevOffset = offset;
	}
}
//=============================================================================
// X�t�@�C���p�X�̓ǂݍ���
//=============================================================================
void CBlockManager::LoadConfig(const std::string& filename)
{
	std::ifstream ifs(filename);
	if (!ifs)
	{
		return;
	}

	json j;
	ifs >> j;

	// j �͔z��ɂȂ��Ă�̂Ń��[�v����
	for (auto& block : j)
	{
		int typeInt = block["type"];
		std::string filepath = block["filepath"];

		s_FilePathMap[(CBlock::TYPE)typeInt] = filepath;
	}
}
//=============================================================================
// �}�b�v�����_����������
//=============================================================================
void CBlockManager::GenerateRandomMap(int seed)
{
	srand(seed);

	// �ǔz�u���̓ǂݍ���
	LoadFromJson("data/wall_info.json");

	// --- �����p�����[�^ ---
	const int GRID_X = 12;
	const int GRID_Z = 12;
	const float AREA_SIZE = 80.0f; // 1�G���A�̍L��

	// ���_�𒆐S�ɔz�u���邽�߂̃I�t�Z�b�g�v�Z
	const float offsetX = -(GRID_X * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;
	const float offsetZ = -(GRID_Z * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;

	// �N���X�^�̐�
	const int clusterCount = 8;

	// �Γ��Ă̎c�萔
	int torchRemaining = 2 ;

	// ���Ă̈ʒu��ۑ����郊�X�g
	std::vector<D3DXVECTOR3> torchPositions;

	// �ǂ��痣���}�X
	int margin = 0;

	// �i�s�����i0 = X�������ɉ��f, 1 = Z�������ɏc�f�j
	bool verticalRiver = (rand() % 2 == 0);

	int z = margin + rand() % (GRID_Z - 2 * margin);
	int x = margin + rand() % (GRID_X - 2 * margin);

	// --- �쐶�� ---
	const int riverWidth = 2;

	if (!verticalRiver)
	{
		// --- X�����ɉ��f ---
		for (int i = margin; i < GRID_X - margin; i++)
		{
			for (int w = 0; w < riverWidth; w++)
			{
				D3DXVECTOR3 pos(offsetX + i * AREA_SIZE, -48.0f, offsetZ + (z + w) * AREA_SIZE);
				CreateBlock(CBlock::TYPE_WATER, pos);
			}

			// Z�����֍s
			z += (rand() % 3) - 1; // -1,0,+1

			if (z < margin)
			{
				z = margin;
			}
			if (z >= GRID_Z - margin - riverWidth)
			{
				z = GRID_Z - margin - riverWidth - 1;
			}
		}
	}
	else
	{
		// --- Z�����ɏc�f ---
		for (int i = margin; i < GRID_Z - margin; i++)
		{
			for (int w = 0; w < riverWidth; w++)
			{
				D3DXVECTOR3 pos(offsetX + (x + w) * AREA_SIZE, -48.0f, offsetZ + i * AREA_SIZE);
				CreateBlock(CBlock::TYPE_WATER, pos);
			}

			// X�����֍s
			x += (rand() % 3) - 1;

			if (x < margin)
			{
				x = margin;
			}
			if (x >= GRID_X - margin - riverWidth)
			{
				x = GRID_X - margin - riverWidth - 1;
			}
		}
	}

	// �������u���b�N���W�̎擾
	std::vector<D3DXVECTOR3> waterPositions;
	for (auto block : m_blocks)
	{
		if (block->GetType() == CBlock::TYPE_WATER)
		{
			waterPositions.push_back(block->GetPos());
		}
	}

	for (int i = 0; i < clusterCount; i++)
	{
		// �N���X�^�̒��S���W
		float centerX = offsetX + (rand() % GRID_X) * AREA_SIZE;
		float centerZ = offsetZ + (rand() % GRID_Z) * AREA_SIZE;

		// �N���X�^�͈̔́i���̍L����j
		float radius = 50.0f + rand() % 100;

		// �N���X�^���ɕ����z�u
		int count = 12 + rand() % 10;

		for (int j = 0; j < count; j++)
		{
			float angle = (rand() % 360) * D3DX_PI / 180.0f;
			float dist = (rand() / (float)RAND_MAX) * radius;

			D3DXVECTOR3 pos(
				centerX + cosf(angle) * dist,
				0.0f,
				centerZ + sinf(angle) * dist
			);

			// �O���ɋ߂��Ȃ�u�΂݁v�A�����Ȃ�u���� or �����Ȃ��v
			float distFromCenterX = fabsf(pos.x);
			float distFromCenterZ = fabsf(pos.z);
			float halfWidth = (GRID_X * AREA_SIZE) / 2.0f * 0.9f; // �O������臒l

			CBlock::TYPE type = CBlock::TYPE_GRASS; // �f�t�H���g�͖΂�

			// �O�� �� �΂�
			if (distFromCenterX > halfWidth * 0.6f || distFromCenterZ > halfWidth * 0.6f)
			{
				// ����A���z�u
				int grassLength = 2 + rand() % 5; // 1�`3�}�X
				bool horizontal = rand() % 2;    // X������Z�����������_��

				float minX = offsetX;
				float maxX = offsetX + (GRID_X - 1) * AREA_SIZE;
				float minZ = offsetZ;
				float maxZ = offsetZ + (GRID_Z - 1) * AREA_SIZE;

				for (int k = 0; k < grassLength; k++)
				{
					D3DXVECTOR3 grassPos = pos;
					if (horizontal)
					{
						grassPos.x += k * AREA_SIZE;
					}
					else
					{
						grassPos.z += k * AREA_SIZE;
					}

					// �}�b�v���`�F�b�N
					if (grassPos.x < minX || grassPos.x > maxX ||
						grassPos.z < minZ || grassPos.z > maxZ)
					{
						continue;
					}

					// ���ƏՓ˂��Ă��Ȃ���
					bool collidesWithWater = false;
					for (auto wp : waterPositions)
					{
						if (fabs(wp.x - grassPos.x) < AREA_SIZE * 0.5f &&
							fabs(wp.z - grassPos.z) < AREA_SIZE * 0.5f)
						{
							collidesWithWater = true;
							break;
						}
					}

					if (collidesWithWater)
					{
						continue;
					}

					CBlock* grassBlock = CreateBlock(CBlock::TYPE_GRASS, grassPos);

					if (!grassBlock)
					{
						continue;
					}

					float scaleX = 1.0f + (rand() / (float)RAND_MAX) * 0.8f;
					float scaleY = 1.3f + (rand() / (float)RAND_MAX) * 0.5f;

					// �T�C�Y�̐ݒ�
					grassBlock->SetSize(D3DXVECTOR3(scaleX, scaleY, scaleX));

					float rotY = (rand() % 360) * D3DX_PI / 180.0f;

					// �����̐ݒ�
					grassBlock->SetRot(D3DXVECTOR3(0, rotY, 0));
				}

				continue; // ���̃N���X�^�v�f�͏����ς�
			}
			else if (torchRemaining > 0)
			{
				// ���ē��m�̋����`�F�b�N�i�S�����j
				bool tooClose = false;
				const float MIN_TORCH_DISTANCE = 5.0f * AREA_SIZE; // 5�}�X��

				for (auto& tpos : torchPositions)
				{
					float dx = tpos.x - pos.x;
					float dz = tpos.z - pos.z;
					float distSq = dx * dx + dz * dz;

					if (distSq < (MIN_TORCH_DISTANCE * MIN_TORCH_DISTANCE))
					{
						tooClose = true;
						break;
					}
				}

				if (!tooClose)
				{
					type = CBlock::TYPE_TORCH_01;
					torchRemaining--;
					torchPositions.push_back(pos); // �z�u�L�^
				}
				else
				{
					type = CBlock::TYPE_GRASS;
				}
			}

			// ���Əd�Ȃ�Ȃ��ꍇ�̂ݐ���
			bool collidesWithWater = false;
			for (auto wp : waterPositions)
			{
				if (fabs(wp.x - pos.x) < AREA_SIZE * 0.5f &&
					fabs(wp.z - pos.z) < AREA_SIZE * 0.5f)
				{
					collidesWithWater = true;
					break;
				}
			}

			if (collidesWithWater)
			{
				continue;
			}

			// ����
			CBlock* block = CreateBlock(type, pos);

			if (!block)
			{
				continue;
			}

			// �Γ��ẴI�t�Z�b�g
			if (type == CBlock::TYPE_TORCH_01)
			{
				D3DXVECTOR3 newPos = block->GetPos();
				newPos.y += 35.0f;
				block->SetPos(newPos);
			}

			// �΂݂̃����_������
			if (type == CBlock::TYPE_GRASS)
			{
				// �T�C�Y�������_���ݒ�
				float scaleX = 1.0f + (rand() / (float)RAND_MAX) * 0.8f; // 1.0�`1.8
				float scaleY = 1.3f + (rand() / (float)RAND_MAX) * 0.3f; // 1.3�`1.6

				float scaleZ = scaleX;
				block->SetSize(D3DXVECTOR3(scaleX, scaleY, scaleZ));

				// Y����]�������_���ɐݒ�
				float rotY = (rand() % 360) * D3DX_PI / 180.0f;
				block->SetRot(D3DXVECTOR3(0.0f, rotY, 0.0f));
			}
		}
	}

	// ����~���l�߂鏈��
	FillFloor(GRID_X, GRID_Z, AREA_SIZE); 
}
//=============================================================================
// ����~���l�߂鏈��
//=============================================================================
void CBlockManager::FillFloor(int GRID_X, int GRID_Z, float AREA_SIZE)
{
	float offsetX = -(GRID_X * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;
	float offsetZ = -(GRID_Z * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;

	// ���ɑ��݂��鐅�u���b�N�̈ʒu���`�F�b�N���邽�߃Z�b�g�쐬
	std::vector<D3DXVECTOR3> waterPositions;
	for (auto block : m_blocks)
	{
		if (block->GetType() == CBlock::TYPE_WATER)
		{
			waterPositions.push_back(block->GetPos());
		}
	}

	// �S�}�X����
	for (int x = 0; x < GRID_X; x++)
	{
		for (int z = 0; z < GRID_Z; z++)
		{
			D3DXVECTOR3 pos(offsetX + x * AREA_SIZE, -48.0f, offsetZ + z * AREA_SIZE);

			// �������u���b�N�Əd�Ȃ�Ȃ��ꍇ�̂ݏ�����
			bool occupied = false;
			for (auto wp : waterPositions)
			{
				if (fabs(wp.x - pos.x) < AREA_SIZE * 0.5f && fabs(wp.z - pos.z) < AREA_SIZE * 0.5f)
				{
					occupied = true;
					break;
				}
			}

			if (!occupied)
			{
				// �ʏ폰
				CreateBlock(CBlock::TYPE_FLOOR, pos);
			}
			else
			{
				pos.y = -70.0f;
				CreateBlock(CBlock::TYPE_FLOOR2, pos);
			}
		}
	}
}
//=============================================================================
// �^�C�v����X�t�@�C���p�X���擾
//=============================================================================
const char* CBlockManager::GetFilePathFromType(CBlock::TYPE type)
{
	auto it = s_FilePathMap.find(type);
	return (it != s_FilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// �u���b�N���̕ۑ�����
//=============================================================================
void CBlockManager::SaveToJson(const char* filename)
{
	// JSON�I�u�W�F�N�g
	json j;

	// 1�Â�JSON��
	for (const auto& block : m_blocks)
	{
		json b;
		block->SaveToJson(b);

		// �ǉ�
		j.push_back(b);
	}

	// �o�̓t�@�C���X�g���[��
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// �t�@�C�������
		file.close();
	}
}
//=============================================================================
// �u���b�N���̓ǂݍ��ݏ���
//=============================================================================
void CBlockManager::LoadFromJson(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// �J���Ȃ�����
		return;
	}

	json j;
	file >> j;

	// �t�@�C�������
	file.close();

	// �����̃u���b�N������
	for (auto block : m_blocks)
	{
		if (block != nullptr)
		{
			// �u���b�N�̏I������
			block->Uninit();
		}
	}

	// ���I�z�����ɂ��� (�T�C�Y��0�ɂ���)
	m_blocks.clear();

	// �V���ɐ���
	for (const auto& b : j)
	{
		CBlock::TYPE type = b["type"];
		D3DXVECTOR3 pos(b["pos"][0], b["pos"][1], b["pos"][2]);

		// �u���b�N�̐���
		CBlock* block = CreateBlock(type, pos);

		if (!block)
		{
			continue;
		}

		block->LoadFromJson(b);
	}
}