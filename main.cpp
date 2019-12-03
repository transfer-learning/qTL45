#include "tl45emu.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    TL45emu w;
    w.show();
    return a.exec();
}
