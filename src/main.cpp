#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

// Globaler Status für Mod & Buffer
static bool g_bufferModEnabled = false;
static bool g_bufferedClickPending = false;
static int g_bufferFramesRemaining = 0;

// Maximale Frames, die im Voraus gepuffert werden dürfen (z. B. 4 Frames)
constexpr int MAX_BUFFER_FRAMES = 4;

class $modify(MyPlayLayer, PlayLayer) {
    // Wird in jedem Game-Frame aufgerufen
    void update(float dt) {
        PlayLayer::update(dt);

        // Wenn ein Buffer-Click wartet und die Mod aktiv ist
        if (g_bufferModEnabled && g_bufferedClickPending) {
            auto player = m_player1;
            
            if (player) {
                // Versuche den Sprung auszuführen (PlayLayer steuert das In-Game Handling)
                this->pushButton(1, true);

                // Wenn der Spieler erfolgreich verarbeitet wurde oder das Fenster abläuft
                g_bufferFramesRemaining--;
                if (g_bufferFramesRemaining <= 0) {
                    g_bufferedClickPending = false;
                }
            }
        }
    }

    // Wenn ein Level beendet/neugestartet wird, Buffer zurücksetzen
    void resetLevel() {
        PlayLayer::resetLevel();
        g_bufferedClickPending = false;
        g_bufferFramesRemaining = 0;
    }
};

class $modify(MyKeyboardDispatcher, CCKeyboardDispatcher) {
    // Tastatureingaben abfangen (Toggle & Input-Buffering)
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat) {
        // Toggle mit der Taste 'B'
        if (key == KEY_B && isKeyDown && !isKeyRepeat) {
            g_bufferModEnabled = !g_bufferModEnabled;
            
            std::string msg = g_bufferModEnabled ? "Buffer Mod: ON" : "Buffer Mod: OFF";
            Notification::create(msg, NotificationIcon::Info)->show();
            return true;
        }

        // Wenn Mod aktiv ist und die Leertaste / Pfeil-Nach-Oben gedrückt wird
        if (g_bufferModEnabled && (key == KEY_Space || key == KEY_Up) && isKeyDown && !isKeyRepeat) {
            g_bufferedClickPending = true;
            g_bufferFramesRemaining = MAX_BUFFER_FRAMES;
        }

        // Standard-Behandlung der Tasten
        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat);
    }
};
