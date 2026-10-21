#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>

using namespace geode::prelude;

// Status-Variablen für den Mod
static bool g_bufferModEnabled = true;
// Verfügbare Ticks/Frames zum Durchschalten (Off/0, 2, 4, 8, 16 Ticks)
static std::vector<int> g_tickOptions = {0, 2, 4, 8, 16};
static size_t g_currentTickIndex = 2; // Startet standardmäßig bei 4 Ticks

// Puffer-Status pro Frame-Update
static bool g_bufferedActionPending = false;
static int g_ticksRemaining = 0;

class $modify(MyPlayLayer, PlayLayer) {
    void update(float dt) {
        PlayLayer::update(dt);

        // Wenn ein Input im Puffer wartet und Ticks aktiv sind
        int activeTicks = g_tickOptions[g_currentTickIndex];
        if (g_bufferModEnabled && activeTicks > 0 && g_bufferedActionPending) {
            if (m_player1) {
                // Führt den Sprung/Klick verlässlich aus
                m_player1->pushButton(PlayerButton::Jump);
            }

            g_ticksRemaining--;
            if (g_ticksRemaining <= 0) {
                g_bufferedActionPending = false;
            }
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        g_bufferedActionPending = false;
        g_ticksRemaining = 0;
    }
};

class $modify(MyKeyboardDispatcher, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isKeyDown, bool isKeyRepeat, double timestamp) {
        
        // Mit Taste 'B' durch die Ticks schalten
        if (key == KEY_B && isKeyDown && !isKeyRepeat) {
            g_currentTickIndex = (g_currentTickIndex + 1) % g_tickOptions.size();
            int currentTicks = g_tickOptions[g_currentTickIndex];

            std::string msg;
            if (currentTicks == 0) {
                msg = "Buffer Mod: OFF (Legit)";
            } else {
                msg = fmt::format("Buffer Mod: ON (Saved Ticks: {})", currentTicks);
            }
            
            Notification::create(msg, NotificationIcon::Info)->show();
            return true;
        }

        // Wenn der Spieler klickt (Leertaste / Pfeil hoch), wird der Input abgefangen und gepuffert
        int activeTicks = g_tickOptions[g_currentTickIndex];
        if (g_bufferModEnabled && activeTicks > 0 && (key == KEY_Space || key == KEY_Up) && isKeyDown && !isKeyRepeat) {
            g_bufferedActionPending = true;
            g_ticksRemaining = activeTicks;
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isKeyDown, isKeyRepeat, timestamp);
    }
};
