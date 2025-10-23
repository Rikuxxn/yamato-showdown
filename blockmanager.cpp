//=============================================================================
//
// ブロックマネージャー処理 [blockmanager.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// インクルードファイル
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

// JSONの使用
using json = nlohmann::json;

//*****************************************************************************
// 静的メンバ変数宣言
//*****************************************************************************
std::vector<CBlock*> CBlockManager::m_blocks = {};	// ブロックの情報
int CBlockManager::m_selectedIdx = 0;				// 選択中のインデックス
CBlock* CBlockManager::m_draggingBlock = {};		// ドラッグ中のブロック
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_FilePathMap; 
std::unordered_map<CBlock::TYPE, std::string> CBlockManager::s_texFilePathMap;
CBlock* CBlockManager::m_selectedBlock = {};// 選択したブロック

//=============================================================================
// コンストラクタ
//=============================================================================
CBlockManager::CBlockManager()
{
	// 値のクリア
	m_selectedBlock				= nullptr;
	m_prevSelectedIdx			= -1;
	m_hasConsumedPayload		= false;
	m_pDebug3D					= nullptr;// 3Dデバッグ表示へのポインタ
	m_autoUpdateColliderSize	= true;
	m_texIDs					= {};
	m_isDragging				= false;// ドラッグ中かどうか
}
//=============================================================================
// デストラクタ
//=============================================================================
CBlockManager::~CBlockManager()
{
	// なし
}
//=============================================================================
// 生成処理
//=============================================================================
CBlock* CBlockManager::CreateBlock(CBlock::TYPE type, D3DXVECTOR3 pos)
{
	const char* path = CBlockManager::GetFilePathFromType(type);

	CBlock* newBlock = CBlock::Create(path, pos, D3DXVECTOR3(0, 0, 0), D3DXVECTOR3(1, 1, 1), type);

	if (newBlock)
	{
		m_blocks.push_back(newBlock);
	}

	return newBlock;
}
//=============================================================================
// 初期化処理
//=============================================================================
void CBlockManager::Init(void)
{
	LoadConfig("data/block_payload.json");

	m_texIDs.resize(CBlock::TYPE_MAX);

	// GUIテクスチャの登録
	for (int typeInt = 0; typeInt < (int)CBlock::TYPE_MAX; typeInt++)
	{
		CBlock::TYPE type = static_cast<CBlock::TYPE>(typeInt);
		const char* texPath = CBlockManager::GetTexPathFromType(type);

		if (texPath)
		{
			m_texIDs[typeInt] = CManager::GetTexture()->RegisterDynamic(texPath);
		}
		else
		{
			m_texIDs[typeInt] = -1; // 無効扱い
		}
	}

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
}
//=============================================================================
// 終了処理
//=============================================================================
void CBlockManager::Uninit(void)
{
	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();
}
//=============================================================================
// 削除予約があるブロックの削除処理
//=============================================================================
void CBlockManager::CleanupDeadBlocks(void)
{
	for (int i = (int)m_blocks.size() - 1; i >= 0; i--)
	{
		if (m_blocks[i]->IsDead())
		{
			// ブロックの終了処理
			m_blocks[i]->Uninit();
			m_blocks.erase(m_blocks.begin() + i);
		}
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void CBlockManager::Update(void)
{
#ifdef _DEBUG

	// 情報の更新
	UpdateInfo();

#endif
	// ブロック削除処理
	CleanupDeadBlocks();
}
//=============================================================================
// 描画処理
//=============================================================================
void CBlockManager::Draw(void)
{
#ifdef _DEBUG
	//// 選択中のブロックだけコライダー描画
	//CBlock* pSelectBlock = GetSelectedBlock();
	//if (pSelectBlock != nullptr)
	//{
	//	pSelectBlock->DrawCollider();
	//}
#endif
}
//=============================================================================
// 情報の更新処理
//=============================================================================
void CBlockManager::UpdateInfo(void)
{
	// GUIスタイルの取得
	ImGuiStyle& style = ImGui::GetStyle();

	style.GrabRounding		= 10.0f;		// スライダーのつまみを丸く
	style.ScrollbarRounding = 10.0f;		// スクロールバーの角
	style.ChildRounding		= 10.0f;		// 子ウィンドウの角
	style.WindowRounding	= 10.0f;		// ウィンドウ全体の角

	// 場所
	CImGuiManager::Instance().SetPosImgui(ImVec2(1900.0f, 20.0f));

	// サイズ
	CImGuiManager::Instance().SetSizeImgui(ImVec2(420.0f, 500.0f));

	// 最初のGUI
	CImGuiManager::Instance().StartImgui(u8"BlockInfo", CImGuiManager::IMGUITYPE_DEFOULT);

	// ブロックがない場合
	if (m_blocks.empty())
	{
		ImGui::Text("No blocks placed yet.");
	}
	else
	{
		// ブロックの総数
		ImGui::Text("Block Num %d", (int)m_blocks.size());

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// インデックス選択
		ImGui::SliderInt("Block Index", &m_selectedIdx, 0, (int)m_blocks.size() - 1);

		// 範囲外対策
		if (m_selectedIdx >= (int)m_blocks.size())
		{
			m_selectedIdx = (int)m_blocks.size() - 1;
		}

		// 前回選んでたブロックを解除
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != m_selectedIdx)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// 対象ブロックの取得
		m_selectedBlock = m_blocks[m_selectedIdx];

		// ブロック情報の調整処理
		UpdateTransform(m_selectedBlock);

		if (GetUpdateCollider() == false)
		{
			// コライダーの調整処理
			UpdateCollider(m_selectedBlock);
		}
		else
		{
			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける
			ImGui::Text("Auto Update Collider Size is Active");
		}
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	// ブロックタイプ一覧
	if (ImGui::TreeNode("Block Types"))
	{
		ImGui::BeginChild("BlockTypeList", ImVec2(0, 500), true); // スクロール領域

		const float imageSize = 100.0f; // 大きめ画像
		int numTypes = (int)CBlock::TYPE_MAX;

		for (int i = 0; i < numTypes; i++)
		{
			if (i >= static_cast<int>(m_texIDs.size()))
			{
				continue;
			}

			int texIdx = m_texIDs[i];
			LPDIRECT3DTEXTURE9 tex = CManager::GetTexture()->GetAddress(texIdx);

			if (!tex)
			{
				continue; // nullptr はスキップ
			}

			ImGui::PushID(i);
			ImGui::Image(reinterpret_cast<ImTextureID>(tex), ImVec2(imageSize, imageSize));

			if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
			{
				m_isDragging = true;// ドラッグ中
				CBlock::TYPE payloadType = static_cast<CBlock::TYPE>(i);
				ImGui::SetDragDropPayload("BLOCK_TYPE", &payloadType, sizeof(payloadType));
				ImGui::Text("Block Type %d", i);
				ImGui::Image(reinterpret_cast<ImTextureID>(tex), ImVec2(imageSize, imageSize));
				ImGui::EndDragDropSource();
			}

			// マウスの取得
			CInputMouse* pMouse = CManager::GetInputMouse();

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && m_isDragging)
			{
				// 現在のImGuiの内部状態（コンテキスト）へのポインターを取得
				ImGuiContext* ctx = ImGui::GetCurrentContext();

				if (ctx->DragDropPayload.IsDataType("BLOCK_TYPE"))
				{
					// ドラッグ中を解除
					m_isDragging = false;

					CBlock::TYPE draggedType = *(CBlock::TYPE*)ctx->DragDropPayload.Data;

					D3DXVECTOR3 pos = pMouse->GetGroundHitPosition();
					pos.y = 30.0f;

					// ブロックの生成
					m_draggingBlock = CreateBlock(draggedType, pos);
				}
			}

			ImGui::PopID();

			ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 画像間の余白
		}

		ImGui::EndChild();
		ImGui::TreePop();
	}

	ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

	if (ImGui::Button("Save"))
	{
		// ダイアログを開いてファイルに保存
		std::string path = OpenWindowsSaveFileDialog();

		if (!path.empty())
		{
			// データの保存
			CBlockManager::SaveToJson(path.c_str());
		}
	}

	ImGui::SameLine(0);

	if (ImGui::Button("Load"))
	{
		// ダイアログを開いてファイルを開く
		std::string path = OpenWindowsOpenFileDialog();

		if (!path.empty())
		{
			// データの読み込み
			CBlockManager::LoadFromJson(path.c_str());
		}
	}

	ImGui::End();

	if (CManager::GetMode() == MODE_TITLE)
	{
		return;
	}

	// マウス選択処理
	PickBlockFromMouseClick();
}
//=============================================================================
// ブロック情報の調整処理
//=============================================================================
void CBlockManager::UpdateTransform(CBlock* selectedBlock)
{
	if (selectedBlock)
	{
		// 選択中のブロックの色を変える
		selectedBlock->SetSelected(true);

		D3DXVECTOR3 pos = selectedBlock->GetPos();	// 選択中のブロックの位置の取得
		D3DXVECTOR3 rot = selectedBlock->GetRot();	// 選択中のブロックの向きの取得
		D3DXVECTOR3 size = selectedBlock->GetSize();// 選択中のブロックのサイズの取得
		btScalar mass = selectedBlock->GetMass();	// 選択中のブロックの質量の取得

		// ラジアン→角度に一時変換（静的変数で保持し操作中のみ更新）
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
			selectedBlock->SetEditMode(true);  // チェックでKinematic化
		}
		else
		{
			selectedBlock->SetEditMode(false); // 通常に戻す
		}

		//*********************************************************************
		// POS の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("POS"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

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
		// ROT の調整
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
		// SIZE の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("SIZE"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

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

		// チェックボックス：拡大率に応じて自動更新するか
		ImGui::Checkbox("Auto Update Collider Size", &m_autoUpdateColliderSize);

		// 前回のサイズを保持
		static D3DXVECTOR3 prevSize = selectedBlock->GetSize();

		// サイズ変更チェック
		bool isSizeChanged = (size != prevSize);

		//*********************************************************************
		// 質量 の調整
		//*********************************************************************

		ImGui::Dummy(ImVec2(0.0f, 10.0f)); // 空白を空ける

		// ラベル
		ImGui::Text("MASS"); ImGui::SameLine(60); // ラベルの位置ちょっと調整

		// スライダー（範囲: 0.0f ～ 80.0f）
		ImGui::SliderFloat("##MassSlider", &mass, 0.0f, 80.0f, "%.2f");


		// 角度→ラジアンに戻す
		D3DXVECTOR3 rotRad = D3DXToRadian(degRot);

		// 位置の設定
		selectedBlock->SetPos(pos);

		// サイズの設定
		selectedBlock->SetSize(size);

		// サイズ(拡大率)が変わったときだけ呼ぶ
		if (m_autoUpdateColliderSize == true && isSizeChanged)
		{
			// ブロックサイズによるコライダーの生成
			selectedBlock->CreatePhysicsFromScale(size);

			prevSize = size; // 更新
		}

		if (isRotChanged)
		{
			// 回転が変わった時だけセット
			selectedBlock->SetRot(rotRad);

			// 編集モードなら即物理Transformも更新して同期
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

			// シーソーブロックならヒンジ軸も更新
			if (auto seesaw = dynamic_cast<CSeesawBlock*>(selectedBlock))
			{
				seesaw->RemoveHinge();  // 既存ヒンジ削除
				seesaw->SetHinge();     // 新しい回転に合わせてヒンジ作成
			}
		}
		else
		{
			// 編集していない時はdegRotをselectedBlockの値に同期しておく
			degRot = D3DXToDegree(selectedBlock->GetRot());
		}

		//*********************************************************************
		// ブロックの削除
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
					// 選択中のブロックを削除
					m_blocks[m_selectedIdx]->Uninit();
				}

				m_blocks.erase(m_blocks.begin() + m_selectedIdx);

				// 選択インデックスを調整
				if (m_selectedIdx >= (int)m_blocks.size())
				{
					m_selectedIdx = (int)m_blocks.size() - 1;
				}

				m_prevSelectedIdx = -1;
			}
			else
			{// m_autoUpdateColliderSizeがfalseの時は何もしない
				
			}
		}

		ImGui::PopStyleColor(3);
	}

	// 最後に保存
	m_prevSelectedIdx = m_selectedIdx;
}
//=============================================================================
// ブロック選択処理
//=============================================================================
void CBlockManager::PickBlockFromMouseClick(void)
{
	// ImGuiがマウスを使ってるなら選択処理をキャンセル
	if (ImGui::GetIO().WantCaptureMouse)
	{
		return;
	}

	// 左クリックのみ
	if (!CManager::GetInputMouse()->GetTrigger(0))
	{
		return;
	}

	// レイ取得（CRayCastを使用）
	D3DXVECTOR3 rayOrigin, rayDir;
	CRayCast::GetMouseRay(rayOrigin, rayDir);

	float minDist = FLT_MAX;
	int hitIndex = -1;

	for (size_t i = 0; i < m_blocks.size(); ++i)
	{
		CBlock* block = m_blocks[i];

		// ワールド行列の取得（位置・回転・拡大を含む）
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

	// 選択状態を反映
	if (hitIndex >= 0)
	{
		// 以前選ばれていたブロックを非選択に
		if (m_prevSelectedIdx != -1 && m_prevSelectedIdx != hitIndex)
		{
			m_blocks[m_prevSelectedIdx]->SetSelected(false);
		}

		// 新しく選択
		m_selectedIdx = hitIndex;
		m_blocks[m_selectedIdx]->SetSelected(true);
		m_prevSelectedIdx = hitIndex;
	}
}
//=============================================================================
// コライダーの調整処理
//=============================================================================
void CBlockManager::UpdateCollider(CBlock* selectedBlock)
{
	// 単一コライダー用
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
// Xファイルパスの読み込み
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

	// j は配列になってるのでループする
	for (auto& block : j)
	{
		int typeInt = block["type"];
		std::string filepath = block["filepath"];
		std::string texFilepath = block["GUItexFilepath"];

		s_FilePathMap[(CBlock::TYPE)typeInt] = filepath;
		s_texFilePathMap[(CBlock::TYPE)typeInt] = texFilepath;
	}
}
//=============================================================================
// マップランダム生成処理
//=============================================================================
void CBlockManager::GenerateRandomMap(int seed)
{
	srand(seed);

	// 壁配置情報の読み込み
	LoadFromJson("data/wall_info.json");

	// --- 生成パラメータ ---
	const int GRID_X = 12;
	const int GRID_Z = 12;
	const float AREA_SIZE = 80.0f; // 1エリアの広さ

	// 原点を中心に配置するためのオフセット計算
	const float offsetX = -(GRID_X * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;
	const float offsetZ = -(GRID_Z * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;

	// クラスタの数
	const int clusterCount = 8;

	// 石灯籠の残り数
	int torchRemaining = 2 ;

	// 灯籠の位置を保存するリスト
	std::vector<D3DXVECTOR3> torchPositions;

	// 壁から離すマス
	int margin = 0;

	// 進行方向（0 = X軸方向に横断, 1 = Z軸方向に縦断）
	bool verticalRiver = (rand() % 2 == 0);

	int z = margin + rand() % (GRID_Z - 2 * margin);
	int x = margin + rand() % (GRID_X - 2 * margin);

	// --- 川生成 ---
	const int riverWidth = 2;

	if (!verticalRiver)
	{
		// --- X方向に横断 ---
		for (int i = margin; i < GRID_X - margin; i++)
		{
			for (int w = 0; w < riverWidth; w++)
			{
				D3DXVECTOR3 pos(offsetX + i * AREA_SIZE, -48.0f, offsetZ + (z + w) * AREA_SIZE);
				CreateBlock(CBlock::TYPE_WATER, pos);
			}

			// Z方向蛇行
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
		// --- Z方向に縦断 ---
		for (int i = margin; i < GRID_Z - margin; i++)
		{
			for (int w = 0; w < riverWidth; w++)
			{
				D3DXVECTOR3 pos(offsetX + (x + w) * AREA_SIZE, -48.0f, offsetZ + i * AREA_SIZE);
				CreateBlock(CBlock::TYPE_WATER, pos);
			}

			// X方向蛇行
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

	// 既存水ブロック座標の取得
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
		// クラスタの中心座標
		float centerX = offsetX + (rand() % GRID_X) * AREA_SIZE;
		float centerZ = offsetZ + (rand() % GRID_Z) * AREA_SIZE;

		// クラスタの範囲（島の広がり）
		float radius = 50.0f + rand() % 100;

		// クラスタ内に複数配置
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

			// 外周に近いなら「茂み」、内側なら「灯籠 or 何もなし」
			float distFromCenterX = fabsf(pos.x);
			float distFromCenterZ = fabsf(pos.z);
			float halfWidth = (GRID_X * AREA_SIZE) / 2.0f * 0.9f; // 外周判定閾値

			CBlock::TYPE type = CBlock::TYPE_GRASS; // デフォルトは茂み

			// 外周 → 茂み
			if (distFromCenterX > halfWidth * 0.6f || distFromCenterZ > halfWidth * 0.6f)
			{
				// 草を連続配置
				int grassLength = 2 + rand() % 5; // 1～3マス
				bool horizontal = rand() % 2;    // X方向かZ方向かランダム

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

					// マップ内チェック
					if (grassPos.x < minX || grassPos.x > maxX ||
						grassPos.z < minZ || grassPos.z > maxZ)
					{
						continue;
					}

					// 水と衝突していないか
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

					// サイズの設定
					grassBlock->SetSize(D3DXVECTOR3(scaleX, scaleY, scaleX));

					float rotY = (rand() % 360) * D3DX_PI / 180.0f;

					// 向きの設定
					grassBlock->SetRot(D3DXVECTOR3(0, rotY, 0));
				}

				continue; // このクラスタ要素は処理済み
			}
			else if (torchRemaining > 0)
			{
				// 灯籠同士の距離チェック（全方向）
				bool tooClose = false;
				const float MIN_TORCH_DISTANCE = 5.0f * AREA_SIZE; // 5マス分

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
					torchPositions.push_back(pos); // 配置記録
				}
				else
				{
					type = CBlock::TYPE_GRASS;
				}
			}

			// 水と重ならない場合のみ生成
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

			// 生成
			CBlock* block = CreateBlock(type, pos);

			if (!block)
			{
				continue;
			}

			// 石灯籠のオフセット
			if (type == CBlock::TYPE_TORCH_01)
			{
				D3DXVECTOR3 newPos = block->GetPos();
				newPos.y += 35.0f;
				block->SetPos(newPos);
			}

			// 茂みのランダム調整
			if (type == CBlock::TYPE_GRASS)
			{
				// サイズをランダム設定
				float scaleX = 1.0f + (rand() / (float)RAND_MAX) * 0.8f; // 1.0～1.8
				float scaleY = 1.3f + (rand() / (float)RAND_MAX) * 0.3f; // 1.3～1.6

				float scaleZ = scaleX;
				block->SetSize(D3DXVECTOR3(scaleX, scaleY, scaleZ));

				// Y軸回転をランダムに設定
				float rotY = (rand() % 360) * D3DX_PI / 180.0f;
				block->SetRot(D3DXVECTOR3(0.0f, rotY, 0.0f));
			}
		}
	}

	// 床を敷き詰める処理
	FillFloor(GRID_X, GRID_Z, AREA_SIZE); 
}
//=============================================================================
// 床を敷き詰める処理
//=============================================================================
void CBlockManager::FillFloor(int GRID_X, int GRID_Z, float AREA_SIZE)
{
	float offsetX = -(GRID_X * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;
	float offsetZ = -(GRID_Z * AREA_SIZE) / 2.0f + AREA_SIZE / 2.0f;

	// 既に存在する水ブロックの位置をチェックするためセット作成
	std::vector<D3DXVECTOR3> waterPositions;
	for (auto block : m_blocks)
	{
		if (block->GetType() == CBlock::TYPE_WATER)
		{
			waterPositions.push_back(block->GetPos());
		}
	}

	// 全マス走査
	for (int x = 0; x < GRID_X; x++)
	{
		for (int z = 0; z < GRID_Z; z++)
		{
			D3DXVECTOR3 pos(offsetX + x * AREA_SIZE, -48.0f, offsetZ + z * AREA_SIZE);

			// 既存水ブロックと重ならない場合のみ床生成
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
				// 通常床
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
// タイプからXファイルパスを取得
//=============================================================================
const char* CBlockManager::GetFilePathFromType(CBlock::TYPE type)
{
	auto it = s_FilePathMap.find(type);
	return (it != s_FilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// タイプからGUIテクスチャファイルパスを取得
//=============================================================================
const char* CBlockManager::GetTexPathFromType(CBlock::TYPE type)
{
	auto it = s_texFilePathMap.find(type);
	return (it != s_texFilePathMap.end()) ? it->second.c_str() : "";
}
//=============================================================================
// ブロック情報の保存処理
//=============================================================================
void CBlockManager::SaveToJson(const char* filename)
{
	// JSONオブジェクト
	json j;

	// 1つづつJSON化
	for (const auto& block : m_blocks)
	{
		// ラジアン→角度に一時変換
		D3DXVECTOR3 degRot = D3DXToDegree(block->GetRot());
		
		json b;
		b["type"] = block->GetType();												// ブロックのタイプ
		b["pos"] = { block->GetPos().x, block->GetPos().y, block->GetPos().z };		// 位置
		b["rot"] = { degRot.x, degRot.y, degRot.z };								// 向き
		b["size"] = { block->GetSize().x, block->GetSize().y, block->GetSize().z };	// サイズ

		b["collider_size"] =
		{
			block->GetColliderSize().x,
			block->GetColliderSize().y,
			block->GetColliderSize().z
		};

		// 追加
		j.push_back(b);
	}

	// 出力ファイルストリーム
	std::ofstream file(filename);

	if (file.is_open())
	{
		file << std::setw(4) << j;

		// ファイルを閉じる
		file.close();
	}
}
//=============================================================================
// ブロック情報の読み込み処理
//=============================================================================
void CBlockManager::LoadFromJson(const char* filename)
{
	std::ifstream file(filename);

	if (!file.is_open())
	{// 開けなかった
		return;
	}

	json j;
	file >> j;

	// ファイルを閉じる
	file.close();

	// 既存のブロックを消す
	for (auto block : m_blocks)
	{
		if (block != nullptr)
		{
			// ブロックの終了処理
			block->Uninit();
		}
	}

	// 動的配列を空にする (サイズを0にする)
	m_blocks.clear();

	// 新たに生成
	for (const auto& b : j)
	{
		// タイプ情報を取得して列挙型にキャスト
		CBlock::TYPE typeInt = b["type"];
		CBlock::TYPE type = typeInt;

		D3DXVECTOR3 pos(b["pos"][0], b["pos"][1], b["pos"][2]);
		D3DXVECTOR3 degRot(b["rot"][0], b["rot"][1], b["rot"][2]);
		D3DXVECTOR3 size(b["size"][0], b["size"][1], b["size"][2]);

		D3DXVECTOR3 rot = D3DXToRadian(degRot); // 度→ラジアンに変換

		// タイプからブロック生成
		CBlock* block = CreateBlock(type, pos);

		if (!block)
		{
			continue;
		}

		block->SetRot(rot);
		block->SetSize(size);

		if (b.contains("collider_size"))
		{
			D3DXVECTOR3 colliderSize(
				b["collider_size"][0],
				b["collider_size"][1],
				b["collider_size"][2]);

			block->SetColliderSize(colliderSize);
			block->UpdateCollider(); // 単一用再生成
		}
	}
}
//=============================================================================
// 全ブロックの取得
//=============================================================================
std::vector<CBlock*>& CBlockManager::GetAllBlocks(void)
{
	return m_blocks;
}