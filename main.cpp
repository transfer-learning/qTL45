#include "tl45emu.h"

#include <QApplication>
#include <emulator/tl45/TL45EmulatorState.h>
#include <emulator/scomp/SCOMPEmulator.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "hicpp-signed-bitwise"

int main(int argc, char *argv[]) {
  QApplication a(argc, argv);

  TL45EmulatorState state;
	state.clear(); // allocate virtual memory
  TLEmulator w(&state);
  w.show();
  return a.exec();
}

#pragma clang diagnostic pop
