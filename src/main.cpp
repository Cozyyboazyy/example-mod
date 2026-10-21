// Input-Buffer-Mod (Geode SDK) - Grundgeruest, nur fuer private/offline Nutzung.
// WICHTIG: Signaturen (handleButton, processCommands) gegen die Bindings deiner
// GD/Geode-Version pruefen. Dieser Code ist nicht kompiliert/getestet.

#include <Geode/Geode.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <deque>

using namespace geode::prelude;

struct BufferedInput {
    bool down;        // true = Druecken, false = Loslassen
    int button;       // PlayerButton (Jump = 1, Left = 2, Right = 3)
    bool isPlayer1;
    int ticksWaited = 0;
};

static std::deque<BufferedInput> g_queue;
static bool g_injecting = false; // verhindert, dass wir uns selbst abfangen

// Platzhalter: Hier muesste die Vorhersage rein, ob die Aktion JETZT sicher ist.
// Eine echte Version muesste den Spieler (PlayerObject) einen oder mehrere Ticks
// "trocken" vorwaerts simulieren (Position, Geschwindigkeit, Hitboxen von Hazards/
// Solids pruefen) und den Zustand danach wiederherstellen.
// Solange hier "true" steht, wird der Klick sofort durchgereicht.
static bool isSafeToActNow(GJBaseGameLayer* layer, BufferedInput const& in) {
    return true; // TODO: Vorhersage implementieren
}

class $modify(BufferLayer, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isPlayer1) {
        auto mod = Mod::get();
        if (g_injecting || !mod->getSettingValue<bool>("enabled")) {
            return GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
        // Nur im echten Level puffern, nicht im Editor
        if (!typeinfo_cast<PlayLayer*>(this)) {
            return GJBaseGameLayer::handleButton(down, button, isPlayer1);
        }
        g_queue.push_back({down, button, isPlayer1, 0});
    }

    void processCommands(float dt) {
        auto window = static_cast<int>(Mod::get()->getSettingValue<int64_t>("buffer-ticks"));

        while (!g_queue.empty()) {
            auto& in = g_queue.front();
            bool safe = isSafeToActNow(this, in);
            bool expired = in.ticksWaited >= window;

            if (!safe && !expired) {
                in.ticksWaited++;
                break; // Reihenfolge der Inputs beibehalten, naechsten Tick erneut versuchen
            }

            // sicher ODER Fenster abgelaufen -> Klick ausfuehren
            g_injecting = true;
            GJBaseGameLayer::handleButton(in.down, in.button, in.isPlayer1);
            g_injecting = false;
            g_queue.pop_front();
        }

        GJBaseGameLayer::processCommands(dt);
    }
};

// Queue beim Verlassen/Neustart eines Levels leeren
#include <Geode/modify/PlayLayer.hpp>
class $modify(BufferPlayLayer, PlayLayer) {
    void resetLevel() {
        g_queue.clear();
        PlayLayer::resetLevel();
    }
};
