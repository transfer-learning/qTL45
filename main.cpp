#include "tl45emu.h"

#include <QApplication>
#include <emulator/tl45/TL45EmulatorState.h>
#include <emulator/scomp/SCOMPEmulator.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    TL45EmulatorState state{};
    SCOMPEmulator state{};
    TLEmulator w(&state);
    w.show();
    return a.exec();
}
