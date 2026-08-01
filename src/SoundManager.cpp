#include "SoundManager.hpp"
#include <zip.h>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

SoundManager* SoundManager::get() {
    static SoundManager instance;
    return &instance;
}

bool SoundManager::initialize() {
    auto mod = Mod::get();
    m_soundDir = (mod->getSaveDir() / "sounds").string();
    std::filesystem::create_directories(m_soundDir);

    auto resourcesDir = mod->getResourcesDir();
    auto soundsZip = resourcesDir / "sounds.zip";
    if (std::filesystem::exists(soundsZip)) {
        extractZip(soundsZip, m_soundDir);
    }
    return loadProfile("Viper 8K Default");
}

bool SoundManager::extractZip(const std::filesystem::path& zipPath, const std::filesystem::path& targetDir) {
    int err = 0;
    zip* archive = zip_open(zipPath.string().c_str(), 0, &err);
    if (!archive) return false;

    zip_int64_t numEntries = zip_get_num_entries(archive, 0);
    for (zip_int64_t i = 0; i < numEntries; ++i) {
        const char* name = zip_get_name(archive, i, 0);
        if (!name) continue;

        auto targetPath = targetDir / name;
        std::filesystem::create_directories(targetPath.parent_path());

        zip_file* file = zip_fopen_index(archive, i, 0);
        if (!file) continue;

        std::ofstream out(targetPath.string(), std::ios::binary);
        char buffer[8192];
        zip_int64_t bytesRead;
        while ((bytesRead = zip_fread(file, buffer, sizeof(buffer))) > 0) {
            out.write(buffer, bytesRead);
        }
        out.close();
        zip_fclose(file);
    }
    zip_close(archive);
    return true;
}

bool SoundManager::loadManifest(const std::filesystem::path& manifestPath, SoundProfile& profile) {
    if (!std::filesystem::exists(manifestPath)) return false;

    std::ifstream file(manifestPath.string());
    json data;
    file >> data;

    profile.name = data["name"];
    profile.version = data["version"];
    profile.minPitch = data["pitch"]["min"];
    profile.maxPitch = data["pitch"]["max"];
    profile.referenceCPS = data["pitch"]["reference_cps"];

    for (auto& [key, value] : data["sounds"].items()) {
        profile.soundMap[key] = value;
    }
    return true;
}

bool SoundManager::loadProfile(const std::string& profileName) {
    auto manifestPath = std::filesystem::path(m_soundDir) / profileName / "manifest.json";
    if (!std::filesystem::exists(manifestPath)) return false;

    SoundProfile profile;
    if (!loadManifest(manifestPath, profile)) return false;

    m_profiles[profileName] = profile;
    m_currentProfile = profileName;
    return true;
}

bool SoundManager::loadProfileFromZip(const std::filesystem::path& zipPath) {
    auto tempDir = std::filesystem::temp_directory_path() / "tubaretka_temp";
    std::filesystem::create_directories(tempDir);

    if (!extractZip(zipPath, tempDir)) return false;

    auto manifestPath = tempDir / "manifest.json";
    SoundProfile profile;
    if (!loadManifest(manifestPath, profile)) return false;

    auto targetDir = std::filesystem::path(m_soundDir) / profile.name;
    std::filesystem::create_directories(targetDir);

    for (const auto& entry : std::filesystem::directory_iterator(tempDir)) {
        if (entry.path().extension() == ".wav" || entry.path().extension() == ".ogg") {
            std::filesystem::copy(entry.path(), targetDir / entry.path().filename());
        }
    }
    std::filesystem::copy(manifestPath, targetDir / "manifest.json");
    std::filesystem::remove_all(tempDir);

    m_profiles[profile.name] = profile;
    m_currentProfile = profile.name;
    return true;
}

void SoundManager::playSoundForCPS(float cps) {
    auto& profile = m_profiles[m_currentProfile];
    if (profile.soundMap.empty()) return;

    std::string selectedSound;
    if (cps < 10) selectedSound = profile.soundMap["cps_0_10"];
    else if (cps < 30) selectedSound = profile.soundMap["cps_10_30"];
    else if (cps < 60) selectedSound = profile.soundMap["cps_30_60"];
    else selectedSound = profile.soundMap["cps_60_80"];

    loadSoundFile(selectedSound);

    float pitch = profile.minPitch + (cps / profile.referenceCPS) * (profile.maxPitch - profile.minPitch);
    pitch = std::clamp(pitch, profile.minPitch, profile.maxPitch);
}

void SoundManager::loadSoundFile(const std::string& filename) {
    auto path = std::filesystem::path(m_soundDir) / m_currentProfile / filename;
    if (!std::filesystem::exists(path)) return;
}

void SoundManager::playStraightflySound() {
    auto& profile = m_profiles[m_currentProfile];
    auto it = profile.soundMap.find("straightfly");
    if (it != profile.soundMap.end()) loadSoundFile(it->second);
}

void SoundManager::playReleaseSound() {
    auto& profile = m_profiles[m_currentProfile];
    auto it = profile.soundMap.find("release");
    if (it != profile.soundMap.end()) loadSoundFile(it->second);
}

bool SoundManager::exportSoundPack(const std::string& profileName, const std::filesystem::path& outputPath) {
    auto profileIt = m_profiles.find(profileName);
    if (profileIt == m_profiles.end()) return false;

    auto profileDir = std::filesystem::path(m_soundDir) / profileName;
    if (!std::filesystem::exists(profileDir)) return false;

    int err = 0;
    zip* archive = zip_open(outputPath.string().c_str(), ZIP_CREATE, &err);
    if (!archive) return false;

    for (const auto& entry : std::filesystem::directory_iterator(profileDir)) {
        if (entry.is_regular_file()) {
            zip_source* source = zip_source_file(archive, entry.path().string().c_str(), 0, 0);
            if (source) {
                zip_file_add(archive, entry.path().filename().string().c_str(), source, ZIP_FL_OVERWRITE);
            }
        }
    }
    zip_close(archive);
    return true;
}

bool SoundManager::exportSingleSound(const std::string& soundName, const std::filesystem::path& outputPath) {
    auto& profile = m_profiles[m_currentProfile];
    auto it = profile.soundMap.find(soundName);
    if (it == profile.soundMap.end()) return false;

    auto sourcePath = std::filesystem::path(m_soundDir) / m_currentProfile / it->second;
    if (!std::filesystem::exists(sourcePath)) return false;

    std::filesystem::copy(sourcePath, outputPath);
    return true;
}

std::vector<std::string> SoundManager::getAvailableProfiles() const {
    std::vector<std::string> profiles;
    for (const auto& entry : std::filesystem::directory_iterator(m_soundDir)) {
        if (entry.is_directory() && std::filesystem::exists(entry.path() / "manifest.json")) {
            profiles.push_back(entry.path().filename().string());
        }
    }
    return profiles;
}

void SoundManager::setProfile(const std::string& name) {
    if (m_profiles.find(name) != m_profiles.end()) {
        m_currentProfile = name;
    }
}