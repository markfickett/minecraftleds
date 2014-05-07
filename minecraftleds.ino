#include <DataReceiver.h>
#include <array>
#include <string>

#define NUM_KEYS 6
#define NUM_FIGURES 5
#define UNAVAILABLE_BLINK_PERIOD_MS 1000*3
#define ONLINE_DELAY_MS 1000*5
#define OFFLINE_DELAY_MS 1000*5
#define ONLINE_RISE_MS 200
#define ONLINE_PULSE_MS 2000
#define ONLINE_PULSE_AMPLITUDE 50

DataReceiver<NUM_KEYS> receiver;

/**
 * Controller for the RGB LED associated with a Minecraft character figure.
 */
class FigureLight {
  private:
    const int pinRed, pinGreenBluePwm;
    bool available, online;
    unsigned long onlineChangedMs;
  public:
    FigureLight(int i_pinRed, int i_pinGreenBluePwm) :
        pinRed(i_pinRed), pinGreenBluePwm(i_pinGreenBluePwm),
        online(false), available(false), onlineChangedMs(0) {}

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
      bool nowOnline = strcmp(TRUE_STRING, value) == 0;
      if (online != nowOnline) {
        onlineChangedMs = millis();
      }
      online = nowOnline;
    }

    /**
     * Updates the output LEDs based on server availability and whether the
     * player is online.
     */
    void update(unsigned long t) {
      unsigned long onlineChangeT = (t - onlineChangedMs);
      if (available) {
        float brightness;
        if (onlineChangeT < ONLINE_RISE_MS) {
          // Rises from 0 at onlineChangedMs to 1 at ONLINE_RISE_MS.
          brightness = 255 * sin(HALF_PI * onlineChangeT / ONLINE_RISE_MS);
        } else {
          // Picks up at ONLINE_RISE_MS with a slower pulse.
          brightness = 255 + ONLINE_PULSE_AMPLITUDE * (
              cos(PI * (onlineChangeT - ONLINE_RISE_MS) / ONLINE_PULSE_MS)
              - 1);
        }
        analogWrite(pinGreenBluePwm, online ? (unsigned int)brightness: 0);

        bool redOn =
            (online && (onlineChangeT > ONLINE_DELAY_MS)) ||
            (!online && (onlineChangeT < OFFLINE_DELAY_MS));
        digitalWrite(pinRed, redOn ? HIGH: LOW);
      } else {
        digitalWrite(
            pinRed,
            (((millis() / UNAVAILABLE_BLINK_PERIOD_MS) % 2) == 0) ? HIGH: LOW);
        analogWrite(pinGreenBluePwm, 0);
      }
    }
};

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
  receiver.addKey("Wickedhound", &wickCb);
  receiver.addKey("krc23", &krcCb);
  receiver.addKey("markfickett", &markCb);
  receiver.addKey("other", &otherCb);
  receiver.addKey("available", &availableCb);
  receiver.sendReady();
  for (int i = 0; i < NUM_FIGURES; i++) {
    allFigures[i]->setup();
  }
}

void loop() {
  receiver.readAndUpdate();
  unsigned long t = millis();
  for (int i = 0; i < NUM_FIGURES; i++) {
    allFigures[i]->update(t);
  }
}
