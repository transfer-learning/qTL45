#include "tl45emu.h"

#include <QApplication>
#include <emulator/tl45/TL45EmulatorState.h>
#include <emulator/scomp/SCOMPEmulator.h>

#include <sys/mman.h>
#include <sys/stat.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);
  TL45EmulatorState state{};

  state.state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);
  TLEmulator w(&state);
  w.show();
  return a.exec();
}

#pragma clang diagnostic pop
