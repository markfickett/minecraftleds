#include <DataReceiver.h>
#include <array>
#include <string>

#define NUM_KEYS 5

DataReceiver<NUM_KEYS> receiver;

/**
 * Controller for the RGB LED associated with a Minecraft character figure.
 */
class FigureLight {
  private:
    const int pinRed, pinGreenBluePwm;
    bool available, online;
  public:
    FigureLight(int i_pinRed, int i_pinGreenBluePwm) :
        pinRed(i_pinRed), pinGreenBluePwm(i_pinGreenBluePwm), online(false) {}

    void setup() {
      pinMode(pinRed, OUTPUT);
      pinMode(pinGreenBluePwm, OUTPUT);
    }

    /** Sets whether the server is available. */
    void setAvailable(bool i_available) {
      available = i_available;
    }

    /**
     * Updates whether the character is online.
     *
     * @param value A null-terminated stringification of a Python boolean.
     */
    void operator()(const char* value) {
      online = strcmp(TRUE_STRING, value) == 0;
    }

    /**
     * Updates the output LEDs based on server availability and whether the
     * player is online.
     */
    void update() {
      digitalWrite(pinRed, online ? HIGH: LOW);
      digitalWrite(pinGreenBluePwm, online ? HIGH: LOW);
    }
};

#define NUM_FIGURES 5

FigureLight capy(3, 4);
void capyCb(size_t size, const char* value) { capy(value); }
FigureLight wick(6, 5);
void wickCb(size_t size, const char* value) { wick(value); }
FigureLight krc(8, 9);
void krcCb(size_t size, const char* value) { krc(value); }
FigureLight mark(13, 14);
void markCb(size_t size, const char* value) { mark(value); }
FigureLight other(16, 15);
void otherCb(size_t size, const char* value) { other(value); }

FigureLight* allFigures[] = {&capy, &wick, &krc, &mark, &other};

/** The Minecraft server became available on the network (or went offline). */
void availableCb(size_t size, const char* value) {
  bool available = strcmp(TRUE_STRING, value) == 0;
  for (int i = 0; i < NUM_FIGURES; i++) {
    allFigures[i]->setAvailable(available);
  }
}

void setup() {
  receiver.setup();
  receiver.addKey("CapybaraRancher", &capyCb);
  receiver.addKey("WickedHound", &wickCb);
  receiver.addKey("krc23", &krcCb);
  receiver.addKey("markfickett", &markCb);
  receiver.addKey("other", &otherCb);
  receiver.addKey("available", &availableCb);
  receiver.sendReady();
}

void loop() {
  receiver.readAndUpdate();
  for (int i = 0; i < NUM_FIGURES; i++) {
    allFigures[i]->update();
  }
}
