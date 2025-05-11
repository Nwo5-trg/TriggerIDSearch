#include <Geode/Geode.hpp>
#include <Geode/modify/SetIDPopup.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

std::unordered_set<int> collisionObjects {
    1815, 1816, 3609, 3640
};

std::unordered_set<int> getIntSet(std::string input) {
    std::unordered_set<int> intSet;
    auto start = 0;
    while (true) {
        auto comma = input.find(',', start);
        intSet.insert(std::strtol(input.substr(start, comma - start).c_str(), nullptr, 10));
        if (comma == std::string::npos) break;
        start = comma + 1;
    }
    return intSet;
}


class $modify(IDPopup, SetIDPopup) {
    struct Fields {
        bool findGroups = false;
    };

    bool init(int current, int begin, int end, gd::string title, gd::string button, bool p5, int p6 , float p7, bool p8, bool p9) {
        if(!SetIDPopup::init(current, begin, end, title, button, p5, p6, p7, p8, p9)) return false;
        if (typeinfo_cast<FindObjectPopup*>(this)) {
            auto menu = this->getChildByType<CCLayer>(0)->getChildByType<CCMenu>(0);
            for (int i = 0; i < 3; i++) {
                auto button = CCMenuItemSpriteExtra::create(CCSprite::create(("nwo5.trigger_id_search/button" + std::to_string(i) + ".png").c_str()), this, menu_selector(IDPopup::findTriggers));
                button->setPosition(ccp(86, 0 + (i * 40)));
                button->setScale(0.75);
                button->m_baseScale = 0.75;
                auto valueNode = CCNode::create();
                valueNode->setID(std::to_string(i));
                button->addChild(valueNode);
                menu->addChild(button);
            }
            auto input = this->getChildByType<CCLayer>(0)->getChildByType<CCTextInputNode>(0);
            input->setAllowedChars("1234567890,");
            input->setMaxLabelLength(999);
            auto toggler = CCMenuItemToggler::create(CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png"),
            CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png"), this, menu_selector(IDPopup::onToggleMode));
            toggler->setPosition(ccp(-86, 0));
            menu->addChild(toggler);
        }
        return true;
    }

    void findTriggers(CCObject* sender) {
        int type = stoi(static_cast<CCNode*>(sender)->getChildByType<CCNode>(1)->getID());
        if (m_fields->findGroups) type = -1;
        auto editUI = EditorUI::get();
        auto editor = LevelEditorLayer::get();
        auto objs = type == -1 ? editor->m_objects : editor->m_drawGridLayer->m_effectGameObjects;
        auto groups = getIntSet(std::string(m_inputNode->getString()));
        CCArray foundObjs;
        for (auto obj : CCArrayExt<GameObject*>(objs)) if (hasID(obj, groups, type)) foundObjs.addObject(obj);
            
        // for (int id : getIntVector(input)) { // i could optimise this to o(n) but fuck off
        //     if (id < 1 || id > 9999) continue;
        //     for (auto obj : CCArrayExt<GameObject*>(objs)) {
        //         if (auto trigger = typeinfo_cast<EffectGameObject*>(obj)) {
        //             if (hasID(trigger, id, type)) triggers.addObject(trigger);
        //         }
        //     }
        // }
        if (!mod->getSettingValue<bool>("include-current-selection") || mod->getSettingValue<bool>("auto-delete")) editUI->deselectAll();
        editUI->selectObjects(&foundObjs, true);
        if (mod->getSettingValue<bool>("auto-delete")) editUI->m_trashBtn->activate();
        if (mod->getSettingValue<bool>("update-editor") && !mod->getSettingValue<bool>("auto-delete")) {
            editUI->updateButtons();
            editUI->updateDeleteButtons();
            editUI->updateEditMenu();
            editUI->updateGridNodeSize();
            editUI->updateObjectInfoLabel();
        }
        if (mod->getSettingValue<bool>("close-on-select")) this->getChildByType<CCLayer>(0)->getChildByType<CCMenu>(0)->getChildByType<CCMenuItemSpriteExtra>(3)->activate();
    }

    void onToggleMode(CCObject* sender) {
        m_fields->findGroups = !m_fields->findGroups;
    }

    bool hasID(GameObject* gameObj, std::unordered_set<int> id, int filterType) {
        if (filterType == 0) {
            auto obj = static_cast<EffectGameObject*>(gameObj);
            if (id.contains(obj->m_targetGroupID)) return true;
            if (id.contains(obj->m_centerGroupID)) return true;
            if (id.contains(obj->m_targetModCenterID)) return true;
            if (id.contains(obj->m_rotationTargetID)) return true;
        }
        if (filterType == 1 && !collisionObjects.contains(gameObj->m_objectID)) {
            auto obj = static_cast<EffectGameObject*>(gameObj);
            if (id.contains(obj->m_itemID)) return true;
            if (id.contains(obj->m_itemID2)) return true;
        }
        if (filterType == 2 && collisionObjects.contains(gameObj->m_objectID)) {
            auto obj = static_cast<EffectGameObject*>(gameObj);
            if (id.contains(obj->m_itemID)) return true;
            if (id.contains(obj->m_itemID2)) return true;
        }
        if (filterType == -1) {
            auto groups = gameObj->m_groups;
            if (!groups) return false;
            for (auto group : *groups) if (id.contains(group)) return true;
        }
        return false;
    }   
};