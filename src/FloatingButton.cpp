#include "FloatingButton.hpp"
#include "BotPopup.hpp"

FloatingButton* FloatingButton::create() {
    auto ret = new FloatingButton();
    if (ret && ret->init()) {
        ret->autorelease();
        return ret;
    }
    CC_SAFE_DELETE(ret);
    return nullptr;
}

bool FloatingButton::init() {
    if (!CCLayer::init()) return false;

    auto sprite = CCSprite::create("icon.png");
    if (!sprite) {
        sprite = CCSprite::create("GJ_button_01.png");
        log::warn("icon.png not found in mod root, using default");
    }
    sprite->setScale(0.8f);

    m_button = CCMenuItemSpriteExtra::create(
        sprite,
        this,
        menu_selector(FloatingButton::onOpenMenu)
    );
    
    auto menu = CCMenu::create();
    menu->addChild(m_button);
    this->addChild(menu);

    auto winSize = CCDirector::sharedDirector()->getWinSize();
    setPosition(winSize.width - 60, 60);

    setTouchEnabled(true);
    setTouchMode(kCCTouchesOneByOne);
    m_isDragging = false;
    return true;
}

void FloatingButton::onOpenMenu(CCObject*) {
    BotPopup::create()->show();
}

bool FloatingButton::ccTouchBegan(CCTouch* touch, CCEvent*) {
    auto pos = getParent()->convertToNodeSpace(touch->getLocation());
    if (m_button->boundingBox().containsPoint(pos)) {
        m_offset = pos - getPosition();
        m_isDragging = true;
        return true;
    }
    return false;
}

void FloatingButton::ccTouchMoved(CCTouch* touch, CCEvent*) {
    if (!m_isDragging) return;
    auto pos = getParent()->convertToNodeSpace(touch->getLocation());
    setPosition(pos - m_offset);
}

void FloatingButton::ccTouchEnded(CCTouch*, CCEvent*) {
    m_isDragging = false;
}

void FloatingButton::registerWithTouchDispatcher() {
    CCDirector::sharedDirector()->getTouchDispatcher()->addTargetedDelegate(this, -1, true);
}