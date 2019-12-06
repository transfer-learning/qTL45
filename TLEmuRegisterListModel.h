//
// Created by codetector on 12/3/19.
//
#include <QtCore/QStringListModel>
#include <QtWidgets/QAbstractItemDelegate>
#include <emulator/AbstractEmulatorState.h>
#include "tl45_emu_core.h"

class TLEmuRegisterListModel : public QAbstractListModel{
public:
  TLEmuRegisterListModel(AbstractEmulatorState *machine_state);
  int rowCount(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;
private:
  AbstractEmulatorState *machine_state;
};
