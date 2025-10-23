//=============================================================================
//
// �t�@�C���_�C�A���O���� [FileDialogUtils.cpp]
// Author : RIKU TANEKAWA
//
//=============================================================================

//*****************************************************************************
// �C���N���[�h�t�@�C��
//*****************************************************************************
#include "FileDialogUtils.h"

//=============================================================================
// �t�@�C����ۑ�
//=============================================================================
std::string OpenWindowsSaveFileDialog(void)
{
    CHAR szFile[MAX_PATH] = { 0 };

    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize   = sizeof(OPENFILENAMEA);
    ofn.hwndOwner     = NULL; // �E�B���h�E�n���h��
    ofn.lpstrFilter   = "JSON Files\0*.json\0All Files\0*.*\0"; // �t�@�C���̎�ނ̖��O
    ofn.lpstrFile     = szFile;
    ofn.nMaxFile      = MAX_PATH;
    ofn.Flags         = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
    ofn.lpstrDefExt   = "json";

    if (GetSaveFileNameA(&ofn))
    {
        return std::string(szFile);
    }
    return std::string();
}
//=============================================================================
// �t�@�C�����J��
//=============================================================================
std::string OpenWindowsOpenFileDialog(void)
{
    CHAR szFile[MAX_PATH] = { 0 };

    OPENFILENAMEA ofn = { 0 };
    ofn.lStructSize   = sizeof(OPENFILENAMEA);
    ofn.hwndOwner     = NULL;
    ofn.lpstrFilter   = "JSON Files\0*.json\0All Files\0*.*\0"; // �t�@�C���̎�ނ̖��O
    ofn.lpstrFile     = szFile;
    ofn.nMaxFile      = MAX_PATH;
    ofn.Flags         = OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_EXPLORER;
    ofn.lpstrDefExt   = "json";

    if (GetOpenFileNameA(&ofn))
    {
        return std::string(szFile);
    }
    return std::string();
}
