#include "PDM.h"
#include "USBSerial.h"

mbed::DigitalOut led((PinName)40);

//USBSerial serial;

mbed::UARTSerial serial((PinName)35, (PinName)42, 1000000);

/* 
 * This snippet allows to redirect stdout/stderr on a Stream at your choice
 * Attention: it must be in mbed namespace to override the weak core definition
 */
namespace mbed {
FileHandle *mbed_override_console(int fd) {
  return &serial;
}

FileHandle *mbed_target_override_console(int fd) {
  return &serial;
}
}

uint8_t buffer[1024];
volatile int idx = 0;

void toggle() {
  led = !led;
}

void send(void* buf, size_t size) {
  memcpy(buffer, buf, size);
  idx = 1;
}

void setup() {
  // Start the PDM as MONO @ 16KHz : gain @20
  // At this frequency you have 15ms in the callcack to use the returned buffer
  PDM.begin(1, 16000, 20);
  // The IRQ can call a naked function or one with buffer and size
  PDM.onReceive(send);
  PDM.onReceive(toggle);
}

void loop() {
  if (idx == 1) {
    serial.write(buffer, DEFAULT_PDM_BUFFER_SIZE);
    fflush(stdout);
    idx = 0;
  }
/*
  int available = PDM.available();
  if (available) {
    int ret = PDM.read(buffer, available);
    fwrite(buffer, available, 1, stdout);
    fflush(stdout);
    idx += ret;
  }
*/
}
