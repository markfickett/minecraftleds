#include <DataReceiver.h>
#include <string.h>

#define NUM_KEYS 5

DataReceiver<NUM_KEYS> receiver;

/**
 * Controller for the RGB LED associated with a Minecraft character figure.
 */
class FigureLight {
  private:
    const int pinR, pinGB;
  public:
    FigureLight(int redPin, int greenBluePin) :
        pinR(redPin), pinGB(greenBluePin) {}

    /**
     * Updates whether the character is online.
     *
     * @param value A null-terminated stringification of a Python boolean.
     */
    void operator()(const char* value) {
      if (strcmp(TRUE_STRING, value) == 0) {
      } else {
      }
    }
};

FigureLight capy(1, 2);
void capyCb(size_t size, const char* value) { capy(value); }
FigureLight wick(3, 4);
void wickCb(size_t size, const char* value) { wick(value); }
FigureLight krc(5, 6);
void krcCb(size_t size, const char* value) { krc(value); }
FigureLight mark(7, 8);
void markCb(size_t size, const char* value) { mark(value); }
FigureLight other(9, 10);
void otherCb(size_t size, const char* value) { other(value); }

/** The Minecraft server became available on the network (or went offline). */
void availableCb(size_t size, const char* value) {
  if (strcmp(TRUE_STRING, value) == 0) {
  } else {
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
}
