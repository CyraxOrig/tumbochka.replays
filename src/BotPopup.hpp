#pragma once
#include <Geode/ui/Popup.hpp>
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class BotPopup : public Popup<> {
protected:
    bool setup() override;
    void onToggle(CCObject* sender);
public:
    static BotPopup* create();
};