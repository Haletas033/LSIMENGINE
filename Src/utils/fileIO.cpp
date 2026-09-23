#include <include/utils/fileIO.h>

#include "include/utils/json.h"
#include "include/utils/texture.h"
#include "include/utils/defaults.h"

#ifdef _WIN32
OPENFILENAME ofn;                           //common dialog box structure
char szFile[260] = {"untitled.lsim"};       //File size buffer
HWND hwnd;                                  //owner window
#endif

extern nlohmann::ordered_json config;

extern Defaults engineDefaults;

static Logger logger;

void IO::InitIO() {
    logger = Logger("FILE");
    logger("stdInfo", "Successfully initialized the file loggers");
}

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

void IO::saveToFile(std::ofstream &file, Registry& registry) {
    logger("stdInfo", "Beginning to write to file");

    const auto entities = registry.getAllAlive();

    // Number of entities
    const auto entityCount = static_cast<std::uint32_t>(entities.size());
    file.write(
        reinterpret_cast<const char*>(&entityCount),
        sizeof(entityCount)
    );

    for (const auto& e : entities) {
        // Entity marker
        constexpr std::string_view entityId = "engine.entity";

        constexpr auto entityIdSize = static_cast<std::uint32_t>(entityId.size());

        file.write(
            reinterpret_cast<const char*>(&entityIdSize),
            sizeof(entityIdSize)
        );

        file.write(
            entityId.data(),
            entityId.size()
        );

        for (const auto& type : registry.getComponentTypes(e)) {
            const auto& componentId = registry.getComponentId(type);
            const auto data = registry.serializeComponent(e, type);

            const auto idSize =
                static_cast<std::uint32_t>(componentId.size());

            file.write(
                reinterpret_cast<const char*>(&idSize),
                sizeof(idSize)
            );

            file.write(
                componentId.data(),
                static_cast<std::streamsize>(componentId.size())
            );

            if (!data.empty()) {
                file.write(
                    reinterpret_cast<const char*>(data.data()),
                    static_cast<std::streamsize>(data.size())
                );
            }
        }
    }

    logger("stdInfo", "Successfully wrote to file");
}

void IO::loadFromFile(std::ifstream &file, Registry &registry, SharedState& sharedState, const std::string &workingDir) {
    logger("stdInfo", "beginning to read from file");

    Gui::ClearRoot(registry);
    sharedState.set_current_meshes(registry, {});
    for (const auto e : registry.getAllAlive()) {
        registry.destroyEntity(e);
    }

    const auto data = std::vector<uint8_t>(
        std::istreambuf_iterator(file),
        std::istreambuf_iterator<char>()
    );

    size_t ptr = 0;

    std::uint32_t entityCount;
    std::memcpy(&entityCount, data.data(), sizeof(entityCount));
    ptr += 4;

    std::uint32_t idSize;
    std::memcpy(&idSize, data.data() + ptr, sizeof(idSize));
    ptr += 4;

    std::string_view id(
        reinterpret_cast<const char*>(data.data() + ptr),
        idSize
    );
    ptr += idSize;

    if (id != "engine.entity") { throw std::runtime_error("EXPECTED engine.entity got " + std::string(id) + " instead"); }

    for (std::uint32_t i = 0; i < entityCount; ++i) {
        EntityHandle e = registry.create();

        auto* node = new Gui::Node{ e, Gui::root, {} };
        Gui::root->children.push_back(node);

        while (true) {
            if (ptr == data.size()) {
                break;
            }

            std::memcpy(&idSize, data.data() + ptr, sizeof(idSize));

            ptr += 4;

            id = std::string_view(
                reinterpret_cast<const char*>(data.data() + ptr),
                idSize
            );
            ptr += idSize;

            if (id == "engine.entity") { break; }

            registry.deserializeComponent(e, id, ptr, data);
        }
    }

    logger("stdInfo", "Successfully read file");
}


