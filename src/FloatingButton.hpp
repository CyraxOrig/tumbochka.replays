#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

class FloatingButton : public CCLayer {
protected:
    CCMenuItemSpriteExtra* m_button;
    CCPoint m_offset;
    bool m_isDragging;
public:
    static FloatingButton* create();
    bool init() override;
    void onOpenMenu(CCObject* sender);
    bool ccTouchBegan(CCTouch* touch, CCEvent* event) override;
    void ccTouchMoved(CCTouch* touch, CCEvent* event) override;
    void ccTouchEnded(CCTouch* touch, CCEvent* event) override;
    void registerWithTouchDispatcher() override;
};