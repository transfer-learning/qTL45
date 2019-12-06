//
// Created by codetector on 12/5/19.
//

#ifndef TL45_EMU_TLEMUMEMORYMODEL_H
#define TL45_EMU_TLEMUMEMORYMODEL_H


#include <QtCore/QAbstractListModel>
#include <emulator/AbstractEmulatorState.h>

class TLEmuMemoryModel : public QAbstractListModel {
public:
  explicit TLEmuMemoryModel(AbstractEmulatorState *state) : state(state), baseAddress(state->getProgramCounterValue()) {}

  int rowCount(const QModelIndex &parent) const override;

  QVariant data(const QModelIndex &index, int role) const override;

private:
  AbstractEmulatorState* state;
  uint64_t baseAddress;

  void getDisplayLimits(uint64_t &min, uint64_t &max) const;

  std::string formatMemoryValue(uint64_t addr) const;
};


#endif //TL45_EMU_TLEMUMEMORYMODEL_H
