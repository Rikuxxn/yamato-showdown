//=============================================================================
//
// キャラクターマネージャー処理 [charactermanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _CHARACTERMANAGER_H_
#define _CHARACTERMANAGER_H_

//*****************************************************************************
// インクルードファイル
//*****************************************************************************
#include "object.h"
#include "guage.h"

// 名前空間stdの使用
using namespace std;

//*****************************************************************************
// キャラクタークラス
//*****************************************************************************
class CCharacter : public CObject
{
public:
	CCharacter(int nPriority = 2);
	~CCharacter();

	HRESULT Init(void) = 0;
	void Uninit(void) = 0;
	void Update(void) = 0;
	void Draw(void) = 0;

	void SetHp(float fHp) { m_fHp = fHp; m_fMaxHp = m_fHp; }
	float GetHp(void) const { return m_fHp; }
	float GetMaxHp(void) const { return m_fMaxHp; }

    // HPゲージの設定処理
	void SetGuages(D3DXVECTOR3 pos, float fWidth, float fHeight)
	{
        m_pFrame = CGuage::Create(CGuage::TYPE_FRAME, pos, fWidth, fHeight);// 枠
        m_pBackGuage = CGuage::Create(CGuage::TYPE_BACKGUAGE, pos, fWidth, fHeight);// バックゲージ
		m_pHpGuage = CGuage::Create(CGuage::TYPE_GUAGE, pos, fWidth, fHeight);// HPゲージ

        // HPを個別管理するためにゲージのターゲットを設定
        m_pFrame->SetTargetCharacter(this);
        m_pBackGuage->SetTargetCharacter(this);
        m_pHpGuage->SetTargetCharacter(this);
	}

    // ダメージ処理
    void Damage(float fDamage)
    {
        m_fHp -= fDamage;

        if (m_fHp <= 0.0f)
        {
            m_fHp = 0.0f;
        }
    }

    // 回復処理
    void Heal(float fHealAmount)
    {
        m_fHp += fHealAmount;

        if (m_fHp >= m_fMaxHp)
        {
            m_fHp = m_fMaxHp;
        }
    }

private:

	float m_fHp;
	float m_fMaxHp;

	CGuage* m_pHpGuage;			// 緑ゲージ
	CGuage* m_pBackGuage;		// 赤ゲージ（遅れて追従）
	CGuage* m_pFrame;			// 枠
};

//*****************************************************************************
// キャラクターマネージャークラス
//*****************************************************************************
class CCharacterManager
{
public:
    // キャラクター追加処理
    void AddCharacter(CCharacter* pChar)
    {
        // リストに追加する
        m_characters.push_back(pChar);
    }

    void Update(void)
    {
        for (auto chara : m_characters)
        {
            // キャラクターの更新処理
            chara->Update();
        }
    }

    void Draw(void)
    {
        for (auto chara : m_characters)
        {
            // キャラクターの描画処理
            chara->Draw();
        }
    }

    void Uninit(void)
    {
        for (auto chara : m_characters)
        {
            // キャラクターの終了処理
            chara->Uninit();
            delete chara;
        }

        // リストのクリア(空にする)
        m_characters.clear();
    }

private:
    std::vector<CCharacter*> m_characters;
};

#endif

