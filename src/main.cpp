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
        bool queued = false;
    };

    void handleButton(bool down, int button, bool isPlayer1) {
        if (g_bufferEnabled && isPlayer1 && button == 1) {
            if (down) {
                // Klick in der Luft: merken statt auslösen
                if (m_player1 && !m_player1->m_isOnGround) {
                    m_fields->queued = true;
                    return;
                }
            } else {
                // Losgelassen: gemerkten Klick verwerfen
                m_fields->queued = false;
            }
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }

    void processCommands(float dt, bool isHalfTick, bool isLastTick) {
        GJBaseGameLayer::processCommands(dt, isHalfTick, isLastTick);
        if (g_bufferEnabled && m_fields->queued && m_player1 && m_player1->m_isOnGround) {
            m_fields->queued = false;
            GJBaseGameLayer::handleButton(true, 1, true);  // beim Landen auslösen
        }
    }
};
