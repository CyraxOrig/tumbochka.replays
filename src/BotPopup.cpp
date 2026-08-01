#include "BotPopup.hpp"
#include "SoundManager.hpp"
#include <Geode/Geode.hpp>
#include <Geode/ui/TextInput.hpp>

using namespace geode::prelude;

bool BotPopup::setup() {
    auto winSize = CCDirector::sharedDirector()->getWinSize();
    auto mod = Mod::get();
    bool isMobile = CCApplication::sharedApplication()->getCurrentPlatform() == kPlatformMobile;

    // ===== ФОН (РОДНОЙ GD ИЛИ КАСТОМНЫЙ) =====
    if (isMobile) {
        auto bg = CCSprite::create("PopupWindow.png"_spr);
        if (bg) {
            bg->setPosition(winSize.width / 2, winSize.height / 2);
            bg->setScale(1.2f);
            this->addChild(bg, -1);
        } else {
            auto layer = CCLayerColor::create(ccc4(0, 0, 0, 180));
            this->addChild(layer, -1);
        }
    } else {
        // Стандартный фон GD (для ПК)
        auto bg = CCSprite::create("GJ_squareBG.png");
        bg->setPosition(winSize.width / 2, winSize.height / 2);
        bg->setScale(1.0f);
        this->addChild(bg, -1);
    }

    // ===== ЗАГОЛОВОК =====
    auto title = CCLabelBMFont::create("TubaretKa Replays", "bigFont.fnt");
    title->setPosition(winSize.width / 2, winSize.height - 35);
    this->addChild(title);

    // ===== ЧЕКБОКСЫ =====
    struct ToggleItem {
        const char* label;
        const char* settingKey;
        bool defaultValue;
    };

    std::vector<ToggleItem> toggles = {
        {"Bot Enabled", "bot_enabled", true},
        {"Auto Clicker", "auto_clicker", false},
        {"Straight Fly (Ship)", "straightfly_ship", false},
        {"Straight Fly (UFO)", "straightfly_ufo", false},
        {"Green Dash Orb", "green_orb", false},
        {"Purple Dash Orb", "purple_orb", false},
        {"Sound Feedback", "sound_feedback", true},
        {"FPS Bypass", "fps_bypass", false},
        {"Render Mode", "render_mode", false}
    };

    auto offSprite = CCSprite::create("checkbox_off.png"_spr);
    auto onSprite = CCSprite::create("checkbox_on.png"_spr);

    if (!offSprite || !onSprite) {
        offSprite = CCSprite::create("GJ_checkOff_001.png");
        onSprite = CCSprite::create("GJ_checkOn_001.png");
    }

    float scale = isMobile ? 0.9f : 0.6f;
    offSprite->setScale(scale);
    onSprite->setScale(scale);

    float yStart = winSize.height - 80;
    float xLeft = winSize.width / 2 - (isMobile ? 220 : 150);
    float yOffset = 0;

    for (const auto& item : toggles) {
        auto toggle = CCMenuItemToggler::create(
            offSprite,
            onSprite,
            this,
            menu_selector(BotPopup::onToggle)
        );
        toggle->setPosition(xLeft, yStart - yOffset);
        toggle->toggle(mod->getSettingValue<bool>(item.settingKey));

        auto menu = CCMenu::create();
        menu->addChild(toggle);
        this->addChild(menu);

        auto label = CCLabelBMFont::create(item.label, "chatFont.fnt");
        label->setPosition(xLeft + (isMobile ? 50 : 35), yStart - yOffset);
        label->setScale(isMobile ? 0.9f : 0.7f);
        label->setAnchorPoint(ccp(0, 0.5f));
        this->addChild(label);

        yOffset += isMobile ? 50 : 32;
    }

    // ===== ПОЛЕ ВВОДА: КЛИКОВ В КАДР =====
    auto cpfLabel = CCLabelBMFont::create("Clicks per frame:", "chatFont.fnt");
    cpfLabel->setPosition(winSize.width / 2 - 60, yStart - yOffset - 10);
    cpfLabel->setScale(0.7f);
    this->addChild(cpfLabel);

    auto cpfInput = TextInput::create(100, "1");
    cpfInput->setPosition(winSize.width / 2 + 40, yStart - yOffset - 10);
    cpfInput->setString(std::to_string(mod->getSettingValue<int>("clicks_per_frame")));
    cpfInput->setScale(0.7f);
    cpfInput->setMaxCharCount(7);
    this->addChild(cpfInput);

    // ===== КНОПКА ЗАКРЫТИЯ =====
    auto closeBtn = CCMenuItemSpriteExtra::create(
        CCSprite::create("GJ_closeBtn_001.png"),
        this,
        menu_selector(BotPopup::onClose)
    );
    auto closeMenu = CCMenu::create();
    closeMenu->addChild(closeBtn);
    closeBtn->setPosition(winSize.width - 30, winSize.height - 30);
    this->addChild(closeMenu);

    return true;
}

void BotPopup::onToggle(CCObject* sender) {
    auto toggle = static_cast<CCMenuItemToggler*>(sender);
    bool isOn = toggle->isOn();

    auto mod = Mod::get();
    std::vector<std::string> keys = {
        "bot_enabled", "auto_clicker", "straightfly_ship", "straightfly_ufo",
        "green_orb", "purple_orb", "sound_feedback", "fps_bypass", "render_mode"
    };

    for (int i = 0; i < keys.size(); i++) {
        if (toggle->getTag() == i) {
            mod->setSettingValue(keys[i], isOn);
            log::info("{} = {}", keys[i], isOn ? "ON" : "OFF");
            break;
        }
    }
}

BotPopup* BotPopup::create() {
    auto ret = new BotPopup();
    bool isMobile = CCApplication::sharedApplication()->getCurrentPlatform() == kPlatformMobile;
    float width = isMobile ? 500 : 350;
    float height = isMobile ? 550 : 400;
    
    if (ret && ret->initAnchored(width, height)) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}