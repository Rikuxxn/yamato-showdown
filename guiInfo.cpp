//=============================================================================
//
// GUI���\������ [guiInfo.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "guiInfo.h"
#include "imguimaneger.h"
#include "player.h"
#include "game.h"

//=============================================================================
// �v���C���[�̃f�o�b�O���̕\������
//=============================================================================
void CGuiInfo::PlayerInfo(void)
{
	if (CGame::GetPlayer() == nullptr || CManager::GetMode() != MODE_GAME)
	{
		return;
	}

	CPlayer* pPlayer = CGame::GetPlayer();
	D3DXVECTOR3 pos = pPlayer->GetPos();
	D3DXVECTOR3 rot = pPlayer->GetRot();
	D3DXVECTOR3 size = pPlayer->GetSize();

	if (ImGui::TreeNode("Player Info"))
	{
		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		//*********************************************************************
		// POS �̏��
		//*********************************************************************

		// �ʒu
		ImGui::Text("POS"); ImGui::SameLine(70); // ���x���ʒu������

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // ��������
		ImGui::DragFloat("##Player_pos_x", &pos.x, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_y", &pos.y, 1.0f, -1000.0f, 1000.0f, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_pos_z", &pos.z, 1.0f, -1000.0f, 1000.0f, "%.1f");

		//*********************************************************************
		// ROT �̏��
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ����
		ImGui::Text("ROT"); ImGui::SameLine(70); // ���x���ʒu������

		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80); // ��������
		ImGui::DragFloat("##Player_rot_x", &rot.x, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_rot_y", &rot.y, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_rot_z", &rot.z, 0.1f, -D3DX_PI, D3DX_PI, "%.1f");

		//*********************************************************************
		// SIZE �̏��
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���x��
		ImGui::Text("SIZE"); ImGui::SameLine(70); // ���x���̈ʒu������ƒ���

		// X
		ImGui::Text("X:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_x", &size.x, 0.1f, -100.0f, 100.0f, "%.1f");

		// Y
		ImGui::SameLine();
		ImGui::Text("Y:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_y", &size.y, 0.1f, -100.0f, 100.0f, "%.1f");

		// Z
		ImGui::SameLine();
		ImGui::Text("Z:"); ImGui::SameLine();
		ImGui::SetNextItemWidth(80);
		ImGui::DragFloat("##Player_size_z", &size.z, 0.1f, -100.0f, 100.0f, "%.1f");

		ImGui::TreePop(); // ����
	}
}
//=============================================================================
// �J�����̃f�o�b�O���̕\������
//=============================================================================
void CGuiInfo::CameraInfo(void)
{
	CCamera*pCamera = CManager::GetCamera();
	D3DXVECTOR3 posV = pCamera->GetPosV();
	D3DXVECTOR3 posR = pCamera->GetPosR();
	D3DXVECTOR3 rot = pCamera->GetRot();

	if (ImGui::TreeNode("Camera Info"))
	{
		ImGui::Text("Change Camera Mode : [Press C]");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		ImGui::Text("PosV Move : [Press L ALT & MouseButton L]");
		ImGui::Text("PosR Move : [Press MouseButton R]");
		ImGui::Text("Zoom : [MouseWheel]");

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // �󔒂��󂯂�

		// ���_�̈ʒu
		ImGui::Text("PosV [X:%.1f Y:%.1f Z:%.1f]", posV.x, posV.y, posV.z);

		// �����_�̈ʒu
		ImGui::Text("PosR [X:%.1f Y:%.1f Z:%.1f]", posR.x, posR.y, posR.z);

		// �p�x
		ImGui::Text("Rot  [X:%.2f Y:%.2f Z:%.2f]", rot.x, rot.y, rot.z);

		ImGui::TreePop(); // ����
	}
}