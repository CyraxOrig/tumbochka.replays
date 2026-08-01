#include <Geode/Geode.hpp>
#include "FloatingButton.hpp"
#include "BotPopup.hpp"
#include "SoundManager.hpp"

using namespace geode::prelude;

static FloatingButton* floatingBtn = nullptr;

$on_mod(Loaded) {
    auto mod = Mod::get();
    
    SoundManager::get()->initialize();

    if (CCApplication::sharedApplication()->getCurrentPlatform() == kPlatformMobile) {
        floatingBtn = FloatingButton::create();
        CCScene::get()->addChild(floatingBtn, 999);
    }

    log::info("TubaretKa Replays loaded!");
}

$on_mod(KeyDown) {
    auto hotkey = Mod::get()->getSettingValue<std::string>("hotkey");
    if (hotkey == "Tab") {
        if (!BotPopup::isVisible()) {
            BotPopup::create()->show();
        }
    }
}