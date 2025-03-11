#include <Geode/Geode.hpp>
#include <Geode/modify/SetIDPopup.hpp>

using namespace geode::prelude;

auto mod = Mod::get();

std::unordered_set<int> collisionObjects {
    1815, 1816, 3609, 3640
};

class $modify(IDPopup, SetIDPopup) {
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
        }
        return true;
    }

    void findTriggers(CCObject* sender) {
        int type = stoi(static_cast<CCNode*>(sender)->getChildByType<CCNode>(1)->getID());
        int id = std::stoi(std::string(m_inputNode->getString()));
        if (id < 1 || id > 9999) return;
        CCArray triggers;
        auto objs = LevelEditorLayer::get()->m_objects;
        for (int i = 0; i < objs->count(); i++) {
            if (auto obj = typeinfo_cast<EffectGameObject*>(objs->objectAtIndex(i))) {
                if (hasID(obj, id, type)) triggers.addObject(obj);
            }
        }
        auto editUI = EditorUI::get();
        if (!mod->getSettingValue<bool>("include-current-selection") || mod->getSettingValue<bool>("auto-delete")) editUI->deselectAll();
        editUI->selectObjects(&triggers, true);
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

    bool hasID(EffectGameObject* obj, int id, int filterType) {
        if (filterType == 0) {
            if (obj->m_targetGroupID == id) return true;
            if (obj->m_centerGroupID == id) return true;
            if (obj->m_targetModCenterID == id) return true;
            if (obj->m_rotationTargetID == id) return true;
        }
        if (filterType == 1 && !collisionObjects.contains(obj->m_objectID)) {
            if (obj->m_itemID == id) return true;
            if (obj->m_itemID2 == id) return true;
        }
        if (filterType == 2 && collisionObjects.contains(obj->m_objectID)) {
            if (obj->m_itemID == id) return true;
            if (obj->m_itemID2 == id) return true;
        }
        return false;
    }
};