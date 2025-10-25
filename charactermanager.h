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
#include "manager.h"

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

    // 当たり判定の生成
    void CreatePhysics(float radius, float height);

    // Physicsの破棄
    void ReleasePhysics(void);

    // 当たり判定の位置更新
    void UpdateCollider(D3DXVECTOR3 offset);

    // 向き補間処理
    void UpdateRotation(float fInterpolationSpeed);

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
        if (m_isDead)
        {
            return; // すでに死んでいれば処理しない
        }

        m_fHp -= fDamage;

        if (m_fHp <= 0.0f)
        {
            m_isDead = true;// 死んだ
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

    //*****************************************************************************
    // flagment関数
    //*****************************************************************************
    bool IsDead(void) { return m_isDead; }

    //*****************************************************************************
    // setter関数
    //*****************************************************************************
    void SetHp(float fHp) { m_fHp = fHp; m_fMaxHp = m_fHp; }
    void SetPos(D3DXVECTOR3 pos) { m_pos = pos; }
    void SetRot(D3DXVECTOR3 rot) { m_rot = rot; }
    void SetRotDest(const D3DXVECTOR3& rotDest) { m_rotDest = rotDest; }

    //*****************************************************************************
    // getter関数
    //*****************************************************************************
    D3DXVECTOR3 GetPos(void) { return m_pos; }
    D3DXVECTOR3 GetRot(void) { return m_rot; };
    const D3DXVECTOR3& GetRotDest(void) const { return m_rotDest; }
    D3DXVECTOR3 GetSize(void) { return m_size; }
    float GetHp(void) const { return m_fHp; }
    float GetMaxHp(void) const { return m_fMaxHp; }
    btScalar GetRadius(void) const { return m_radius; }
    btScalar GetHeight(void) const { return m_height; }
    btRigidBody* GetRigidBody(void) const { return m_pRigidBody; }						// RigidBodyの取得
    btCollisionShape* GetCollisionShape(void) { return m_pShape; }
    D3DXVECTOR3 GetColliderPos(void) const { return m_colliderPos; }

private:

	float m_fHp;                // HP
	float m_fMaxHp;             // HP最大量
    bool m_isDead;              // 死んだかどうか
	CGuage* m_pHpGuage;			// 緑ゲージ
	CGuage* m_pBackGuage;		// 赤ゲージ（遅れて追従）
	CGuage* m_pFrame;			// 枠
    D3DXVECTOR3 m_pos;			// 位置
    D3DXVECTOR3 m_rot;			// 向き
    D3DXVECTOR3 m_rotDest;		// 向き
    D3DXVECTOR3 m_size;			// サイズ
    btRigidBody* m_pRigidBody;	// 剛体へのポインタ
    btCollisionShape* m_pShape;	// 当たり判定の形へのポインタ
    btScalar m_radius;			// カプセルコライダーの半径
    btScalar m_height;			// カプセルコライダーの高さ
    D3DXVECTOR3 m_colliderPos;	// カプセルコライダーの位置

};

//*****************************************************************************
// キャラクターマネージャークラス
//*****************************************************************************
class CCharacterManager
{
public:
    //// インスタンスの取得
    //static CCharacterManager& GetInstance(void)
    //{
    //    static CCharacterManager instance;
    //    return instance;
    //}

    // キャラクター追加処理
    void AddCharacter(CCharacter* pChar)
    {
        // リストに追加する
        m_characters.push_back(pChar);
    }

    //// キャラクターの取得処理
    //template <class characterType>
    //characterType* GetCharacter(void)
    //{
    //    for (const auto& c : m_Characters)
    //    {
    //        if (auto casted = dynamic_cast<characterType*>(c))
    //        {
    //            return casted;
    //        }
    //    }
    //    return nullptr;
    //}

    void Init(void)
    {
        for (auto chara : m_characters)
        {
            // キャラクターの更新処理
            chara->Init();
        }
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
    std::vector<CCharacter*> m_characters;// キャラクターのリスト
};

#endif

