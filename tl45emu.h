#ifndef TL45EMU_H
#define TL45EMU_H

#include <QMainWindow>
#include <emulator/AbstractEmulatorState.h>
#include "TLEmuRegisterListModel.h"
#include "tl45_emu_core.h"
#include "TLEmuMemoryModel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class TLEmulator; }
QT_END_NAMESPACE

class TLEmulator : public QMainWindow {
Q_OBJECT

public:
  TLEmulator(AbstractEmulatorState *state, QWidget *parent = nullptr);

  ~TLEmulator();
public slots:
  void onMenuItemClick(bool checked);
  void gotoMemoryClick(bool checked);
private:

  Ui::TLEmulator *ui;
  TLEmuRegisterListModel *registerListModel;
  TLEmuMemoryModel *memoryModel;
  AbstractEmulatorState *machine_state;
};

#endif // TL45EMU_H
