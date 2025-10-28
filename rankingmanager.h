//=============================================================================
//
// �����L���O�}�l�[�W���[���� [rankingmanager.h]
// Author : RIKU TANEKAWA
//
//=============================================================================
#ifndef _RANKINGMANAGER_H_// ���̃}�N����`������Ă��Ȃ�������
#define _RANKINGMANAGER_H_// 2�d�C���N���[�h�h�~�̃}�N����`

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include <algorithm>

//*****************************************************************************
// �����L���O�}�l�[�W���[�N���X
//*****************************************************************************
class CRankingManager
{
private:
    // �����L���O�f�[�^�\����
    struct RankData
    {
        int minutes = 0;
        int seconds = 0;
    };

    static const int MAX_RANK = 5;
    std::vector<RankData> m_rankList;

    static CRankingManager* m_Instance;

public:
	CRankingManager();
	~CRankingManager();

    static CRankingManager* GetInstance(void)
    {
        m_Instance = new CRankingManager();

        return m_Instance;
    }

    // �O������R�s�[�p�Ɏ擾
    std::vector<std::pair<int, int>> GetRankList(void) const
    {
        std::vector<std::pair<int, int>> list;

        for (auto& r : m_rankList)
        {
            list.push_back({ r.minutes, r.seconds });
        }

        return list;
    }

    //*****************************************************************************
    // �ʏ�̃^�C�������̂܂ܓo�^
    //*****************************************************************************
    void AddRecord(int minutes, int seconds)
    {
        RankData data = { minutes, seconds };
        RegisterRecord(data);
    }

    //*****************************************************************************
    // �c�莞�Ԃ��N���A�^�C���ɕϊ����ēo�^
    //*****************************************************************************
    void AddRecordWithLimit(int limitMinutes, int limitSeconds, int remainMinutes, int remainSeconds)
    {
        RankData data = ConvertToClearTime(limitMinutes, limitSeconds, remainMinutes, remainSeconds);
        RegisterRecord(data);
    }

    const std::vector<RankData>& GetList(void) const { return m_rankList; }

    void Save(void);
    void Load(void);

private:
    //*****************************************************************************
    // �c�莞�� �� �N���A�^�C���ւ̕ϊ�
    //*****************************************************************************
    RankData ConvertToClearTime(int limitMinutes, int limitSeconds, int remainMinutes, int remainSeconds)
    {
        int totalLimit = limitMinutes * 60 + limitSeconds;
        int totalRemain = remainMinutes * 60 + remainSeconds;

        int totalClear = totalLimit - totalRemain;
        if (totalClear < 0)
        {
            totalClear = 0;
        }

        RankData data;
        data.minutes = totalClear / 60;
        data.seconds = totalClear % 60;
        return data;
    }

    //*****************************************************************************
    // �����N���X�g�ւ̓o�^�����i���ʁj
    //*****************************************************************************
    void RegisterRecord(const RankData& data)
    {
        m_rankList.push_back(data);

        // �\�[�g�i�������j
        std::sort(m_rankList.begin(), m_rankList.end(), [](const RankData& a, const RankData& b) {
            if (a.minutes == b.minutes)
            {
                return a.seconds < b.seconds;
            }
            return a.minutes < b.minutes;
            });

        // ���5���ɐ���
        if (m_rankList.size() > MAX_RANK)
        {
            m_rankList.resize(MAX_RANK);
        }

        // �ۑ�
        Save();
    }
};

#endif
