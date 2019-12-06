//
// Created by codetector on 12/5/19.
//

#ifndef TL45_EMU_TLEMUMEMORYMODEL_H
#define TL45_EMU_TLEMUMEMORYMODEL_H


#include <QtCore/QAbstractListModel>
#include <emulator/AbstractEmulatorState.h>

class TLEmuMemoryModel : public QAbstractListModel {
public:
  TLEmuMemoryModel(AbstractEmulatorState *state);

  int rowCount(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;

private:
  AbstractEmulatorState* state;
};


#endif //TL45_EMU_TLEMUMEMORYMODEL_H
