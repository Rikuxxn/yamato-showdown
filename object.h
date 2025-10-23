//=============================================================================
//
// �I�u�W�F�N�g���� [object.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _OBJECT_H_// ���̃}�N����`������Ă��Ȃ�������
#define _OBJECT_H_// 2�d�C���N���[�h�h�~�̃}�N����`


//*****************************************************************************
// �I�u�W�F�N�g�N���X
//*****************************************************************************
class CObject
{
public:
	CObject(int nPriority = 3);
	virtual ~CObject();

	// �I�u�W�F�N�g�̎��
	typedef enum
	{
		TYPE_NONE = 0,
		TYPE_PLAYER,
		TYPE_MAX
	}TYPE;

	virtual HRESULT Init(void) = 0;
	virtual void Uninit(void) = 0;
	virtual void Update(void) = 0;
	virtual void Draw(void) = 0;
	static void ReleaseAll(void);
	static void UpdateAll(void);
	static void DrawAll(void);
	static int GetNumObject(void);
	void SetType(TYPE type);
	TYPE GetType(void);
	void Destroy(void);
	bool GetDeath(void) { return m_bDeath; }

protected:
	void Release(void);

private:
	static constexpr int MAX_OBJ_PRIORITY = 8;	// �D�揇��
	static CObject* m_apTop[MAX_OBJ_PRIORITY];	// �擪�̃I�u�W�F�N�g�ւ̃|�C���^
	static CObject* m_apCur[MAX_OBJ_PRIORITY];	// �Ō���̃I�u�W�F�N�g�ւ̃|�C���^
	CObject* m_pPrev;							// �O�̃I�u�W�F�N�g�ւ̃|�C���^
	CObject* m_pNext;							// ���̃I�u�W�F�N�g�ւ̃|�C���^
	static int m_nNumAll;						// �I�u�W�F�N�g�̑���
	TYPE m_type;								// ���
	int m_nPriority;							// �D�揇�ʂ̈ʒu
	bool m_bDeath;								// ���S�t���O
};

#endif
