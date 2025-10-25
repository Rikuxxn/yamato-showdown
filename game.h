//=============================================================================
//
// �Q�[������ [game.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _GAME_H_// ���̃}�N����`������Ă��Ȃ�������
#define _GAME_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "scene.h"
#include "player.h"
#include "time.h"
#include "blockmanager.h"
#include "pausemanager.h"
#include "imguimaneger.h"
#include "grid.h"
#include "enemy.h"

//*****************************************************************************
// �Q�[���N���X
//*****************************************************************************
class CGame : public CScene
{
public:
	CGame();
	~CGame();

	HRESULT Init(void);
	void Uninit(void);
	void Update(void);
	void Draw(void);

	static CPlayer* GetPlayer(void) { return m_pPlayer; }
	static CEnemy* GetEnemy(void) { return m_pEnemy; }
	static CTime* GetTime(void) { return m_pTime; }
	static CBlock* GetBlock(void) { return m_pBlock; }
	static CBlockManager* GetBlockManager(void) { return m_pBlockManager; }
	static CObjectBillboard* GetBillboard(void) { return m_pBillboard; }
	static CPauseManager* GetPauseManager(void) { return m_pPauseManager; }
	static bool GetisPaused(void) { return m_isPaused; };
	static void SetEnablePause(bool bPause);
	static int GetSeed(void) { return m_nSeed; }

private:
	static CPlayer* m_pPlayer;					// �v���C���[�ւ̃|�C���^
	static CEnemy* m_pEnemy;					// �G�ւ̃|�C���^
	static CTime* m_pTime;						// �^�C���ւ̃|�C���^
	static CColon* m_pColon;					// �R�����ւ̃|�C���^
	static CBlock* m_pBlock;					// �u���b�N�ւ̃|�C���^
	static CBlockManager* m_pBlockManager;		// �u���b�N�}�l�[�W���[�ւ̃|�C���^
	static CObjectBillboard* m_pBillboard;		// �r���{�[�h�ւ̃|�C���^
	static CPauseManager* m_pPauseManager;		// �|�[�Y�}�l�[�W���[�ւ̃|�C���^
	std::unique_ptr<CGrid> m_pGrid;				// �O���b�h�ւ̃|�C���^
	static bool m_isPaused;						// true�Ȃ�|�[�Y��
	static int m_nSeed;							// �}�b�v�̃V�[�h�l

};

#endif
