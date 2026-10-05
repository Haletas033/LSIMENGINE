#include "utils/fileIO.h"

#include "utils/json.h"

#ifdef _WIN32
OPENFILENAME ofn;                           //common dialog box structure
char szFile[260] = {"untitled.lsim"};       //File size buffer
HWND hwnd;                                  //owner window
#endif

#ifdef _WIN32
std::string IO::Dialog(const char *filter, const FileDialogFunc func) {
    logger("stdInfo", "Initializing file dialog");
    //Initialize OPENFILENAME
    ZeroMemory(&ofn, sizeof(OPENFILENAME));
    ofn.lStructSize = sizeof(OPENFILENAME);
    ofn.hwndOwner = hwnd;
    ofn.lpstrFile = szFile;
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = filter;
    ofn.nFilterIndex = 1;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR | OFN_OVERWRITEPROMPT;

    // Display the dialog box
    logger("stdInfo", "Displaying file dialog");
    if (func(&ofn) == TRUE) {
        return ofn.lpstrFile;
    }
    logger("stdInfo", "File dialog closed without selecting a file");
    return {};
}

// For directory
std::string IO::DirectoryDialog() {
    logger("stdInfo", "Initializing directory dialog");
    //Initialize OPENFILENAME
    BROWSEINFOA bi = {nullptr};
    bi.lpszTitle = "Select Directory";
    bi.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

    LPITEMIDLIST pidl = SHBrowseForFolderA(&bi);
    if (pidl) {
        char path[MAX_PATH];
        if (SHGetPathFromIDListA(pidl, path)) {
            CoTaskMemFree(pidl);
            return std::string(path);
        }
        CoTaskMemFree(pidl);
    }
    logger("stdInfo", "Directory dialog closed without selecting a directory");
    return {};
}

std::string IO::OpenDialog(const char* filter) {
    return Dialog(filter, GetOpenFileNameA);
}
std::string IO::SaveDialog(const char* filter) {
    return Dialog(filter, GetSaveFileNameA);
}
#else
std::string IO::Dialog(const char *filter) {
    FILE* file = popen("zenity --file-selection", "r");
    if (file == nullptr) return {};
    char filePath[512];
    if (fgets(filePath, 512, file) == nullptr) {
        pclose(file);
        return {};
    }
    pclose(file);
    *strchr(filePath, '\n') = '\0';
    return filePath;
}

std::string IO::DirectoryDialog() {
    FILE* file = popen("zenity --file-selection --directory", "r");
    if (file == nullptr) return {};
    char filePath[512];
    if (fgets(filePath, 512, file) == nullptr) {
        pclose(file);
        return {};
    }
    pclose(file);
    *strchr(filePath, '\n') = '\0';
    return filePath;
}
std::string IO::OpenDialog(const char* filter) {
    return Dialog(filter);
}
std::string IO::SaveDialog(const char* filter) {
    return Dialog(filter);
}
#endif

std::string IO::GetFileContents(const std::string& filePath) {
    std::ifstream file(filePath, std::ios::binary);

    return {
        (std::istreambuf_iterator(file)),
        std::istreambuf_iterator<char>()
    };
}




