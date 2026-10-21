#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

static bool g_bufferEnabled = false;

class $modify(BufferKeys, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool down, bool repeat) {
        if (down && !repeat && key == KEY_Add) {
            g_bufferEnabled = !g_bufferEnabled;
            log::info("Input Buffer: {}", g_bufferEnabled ? "AN" : "AUS");
        }
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, down, repeat);
    }
};

class $modify(BufferLayer, GJBaseGameLayer) {
    struct Fields {
        bool queuedP1 = false;
        bool held = false;
    };

    bool pushButton(int button, bool rightSide) {
        if (!g_bufferEnabled || !m_player1)
            return GJBaseGameLayer::pushButton(button, rightSide);

        m_fields->held = true;
        if (!m_player1->m_isOnGround) {
            m_fields->queuedP1 = true;   // Klick merken
            return false;
        }
        return GJBaseGameLayer::pushButton(button, rightSide);
    }

    bool releaseButton(int button, bool rightSide) {
        m_fields->held = false;
        m_fields->queuedP1 = false;
        return GJBaseGameLayer::releaseButton(button, rightSide);
    }

    void processCommands(float dt) {
        GJBaseGameLayer::processCommands(dt);
        if (g_bufferEnabled && m_fields->queuedP1 && m_player1 && m_player1->m_isOnGround) {
            m_fields->queuedP1 = false;
            GJBaseGameLayer::pushButton(1, true);  // beim Landen auslösen
        }
    }
};
