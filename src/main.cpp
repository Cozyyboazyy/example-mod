#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

// Globaler Status
static bool g_bufferModEnabled = false;
static bool g_bufferedClickPending = false;
static int g_bufferFramesRemaining = 0;

// Maximale Frames für Vorab-Eingabe (3-5 Frames)
constexpr int MAX_BUFFER_FRAMES = 4;

class $modify(MyPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        if (g_bufferModEnabled && g_bufferedClickPending) {
            // In GD 2.2 drückt man den Button am PlayerObject
            if (m_player1) {
                m_player1->pushButton(PlayerButton::Jump);
            }

            g_bufferFramesRemaining--;
            if (g_bufferFramesRemaining <= 0) {
                g_bufferedClickPending = false;
            }
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        g_bufferedClickPending = false;
        g_bufferFramesRemaining = 0;
    }
};

class $modify(MyKeyboardDispatcher, CCKeyboardDispatcher) {
    // Hier ist der 4. Parameter timestamp (double) hinzugefügt
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp) {
        // Toggle mit B
        if (key == KEY_B && isKeyDown && !isKeyRepeat) {
            g_bufferModEnabled = !g_bufferModEnabled;
            
            std::string msg = g_bufferModEnabled ? "Buffer Mod: ON" : "Buffer Mod: OFF";
            Notification::create(msg, NotificationIcon::Info)->show();
            return true;
        }

        // Buffer-Eingabe bei Leertaste oder Pfeil Oben
        if (g_bufferModEnabled && (key == KEY_Space || key == KEY_Up) && isKeyDown && !isKeyRepeat) {
            g_bufferedClickPending = true;
            g_bufferFramesRemaining = MAX_BUFFER_FRAMES;
        }

        // Weiterleiten mit allen 4 Argumenten
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);
    }
};
