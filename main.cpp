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

  FILE *f = fopen("/Users/will/Downloads/a.out", "r");

  struct stat buf{};
  fstat(fileno(f), &buf);

  state.state.memory = (uint8_t *) mmap(nullptr, std::numeric_limits<uint32_t>::max(),
                                        PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_PRIVATE | MAP_NORESERVE, 0, 0);

  fread(state.state.memory, 1, buf.st_size, f);

  printf("MEM[0] = %x\n", state.state.memory[0]);
  printf("MEM[0x1000000] = %x\n", state.state.memory[0x1000000]);



//    SCOMPEmulator state{};
  TLEmulator w(&state);
  w.show();
  return a.exec();
}

#pragma clang diagnostic pop
