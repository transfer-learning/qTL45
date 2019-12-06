//
// Created by codetector on 12/3/19.
//
#include <iostream>
#include <QtWidgets/QLabel>
#include "TLEmuRegisterListModel.h"
#include <fmt/format.h>

int TLEmuRegisterListModel::rowCount(const QModelIndex &parent) const {
  return machine_state->getRegisterCount();
}


std::string combineRegisterNameAndValue(std::string registerName,
                                        uint64_t regBitSize, uint64_t regValue) {
  if (regBitSize <= 4) {
    return fmt::format("{} = 0x{:01X}", registerName, regValue);
  } else if (regBitSize <= 8) {
    return fmt::format("{} = 0x{:02X}", registerName, regValue);
  } else if (regBitSize <= 16) {
    return fmt::format("{} = 0x{:04X}", registerName, regValue);
  } else if (regBitSize <= 32) {
    return fmt::format("{} = 0x{:08X}", registerName, regValue);
  }
  return fmt::format("{} = 0x{:016X}", registerName, regValue);
}

QVariant TLEmuRegisterListModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    uint16_t regId = index.row();
    return QVariant(combineRegisterNameAndValue(machine_state->getRegisterName(regId),
                                                machine_state->getRegisterBitSize(regId),
                                                machine_state->getRegisterValue(regId)).c_str());
  }
  return QVariant();
}

TLEmuRegisterListModel::TLEmuRegisterListModel(AbstractEmulatorState *machine_state) {
  this->machine_state = machine_state;
}
