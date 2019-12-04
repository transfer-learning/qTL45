//
// Created by codetector on 12/3/19.
//
#include <iostream>
#include <QtWidgets/QLabel>
#include "TL45RegisterListModel.h"
#include <fmt/format.h>

int TL45RegisterListModel::rowCount(const QModelIndex &parent) const {
  return machine_state->getRegisterCount();
}


std::string combineRegisterNameAndValue(std::string registerName,
                                        uint64_t regBitSize, uint64_t regValue) {
  return fmt::format("{} = 0x{:08X}", registerName, regValue);
}

QVariant TL45RegisterListModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    uint16_t regId = index.row();
    return QVariant(combineRegisterNameAndValue(machine_state->getRegisterName(regId),
                                                machine_state->getRegisterBitSize(regId),
                                                machine_state->getRegisterValue(regId)).c_str());
  }
  return QVariant();
}

TL45RegisterListModel::TL45RegisterListModel(AbstractEmulatorState *machine_state) {
  this->machine_state = machine_state;
}
