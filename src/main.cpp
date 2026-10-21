#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

// Globale Status-Variablen
static bool g_bufferModEnabled = false;
static std::vector<int> g_bufferFrameOptions = {1, 2, 4, 6, 8, 14, 16};
static size_t g_currentOptionIndex = 2; // Startet bei 4 Frames (Index 2)
static bool g_bufferedClickPending = false;
static int g_bufferFramesRemaining = 0;

class $modify(MyPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        // Wenn die Mod aktiv ist und ein Klick gepuffert wurde
        if (g_bufferModEnabled && g_bufferedClickPending) {
            if (m_player1) {
                // Führt den Sprung/Wave-Click aus, sobald das eingestellte Frame-Fenster greift
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
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp) {
        
        // 1. Mod Ein- und Ausschalten mit Taste 'B'
        if (key == KEY_B && isKeyDown && !isKeyRepeat) {
            g_bufferModEnabled = !g_bufferModEnabled;
            std::string statusMsg = g_bufferModEnabled ? "Buffer Mod: ON" : "Buffer Mod: OFF";
            Notification::create(statusMsg, NotificationIcon::Info)->show();
            return true;
        }

        // 2. Frames durchschalten mit Taste 'T' (1, 2, 4, 6, 8, 14, 16)
        if (key == KEY_T && isKeyDown && !isKeyRepeat) {
            g_currentOptionIndex = (g_currentOptionIndex + 1) % g_bufferFrameOptions.size();
            int currentFrames = g_bufferFrameOptions[g_currentOptionIndex];

            std::string frameMsg = fmt::format("Buffer Frames: {} F", currentFrames);
            Notification::create(frameMsg, NotificationIcon::Success)->show();
            return true;
        }

        // 3. Klick abfangen und puffern basierend auf den ausgewählten Frames
        if (g_bufferModEnabled && (key == KEY_Space || key == KEY_Up) && isKeyDown && !isKeyRepeat) {
            g_bufferedClickPending = true;
            g_bufferFramesRemaining = g_bufferFrameOptions[g_currentOptionIndex];
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);
    }
};
