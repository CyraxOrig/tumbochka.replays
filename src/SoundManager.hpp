#pragma once
#include <Geode/Geode.hpp>
#include <map>
#include <string>
#include <vector>
#include <filesystem>

using namespace geode::prelude;

struct SoundProfile {
    std::string name;
    std::string version;
    std::map<std::string, std::string> soundMap;
    float minPitch;
    float maxPitch;
    float referenceCPS;
};

class SoundManager {
private:
    SoundManager() = default;
    std::map<std::string, SoundProfile> m_profiles;
    std::string m_currentProfile;
    std::string m_soundDir;

    bool extractZip(const std::filesystem::path& zipPath, const std::filesystem::path& targetDir);
    bool loadManifest(const std::filesystem::path& manifestPath, SoundProfile& profile);
    void loadSoundFile(const std::string& filename);

public:
    static SoundManager* get();
    bool initialize();
    bool loadProfile(const std::string& profileName);
    bool loadProfileFromZip(const std::filesystem::path& zipPath);
    void playSoundForCPS(float cps);
    void playStraightflySound();
    void playReleaseSound();
    void setProfile(const std::string& name);
    std::vector<std::string> getAvailableProfiles() const;
    bool importSoundPack(const std::filesystem::path& zipPath);
    bool exportSoundPack(const std::string& profileName, const std::filesystem::path& outputPath);
    bool exportSingleSound(const std::string& soundName, const std::filesystem::path& outputPath);
};