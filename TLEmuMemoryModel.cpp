//
// Created by codetector on 12/5/19.
//

#include "TLEmuMemoryModel.h"

int TLEmuMemoryModel::rowCount(const QModelIndex &parent) const {
  return 9999;
}

QVariant TLEmuMemoryModel::data(const QModelIndex &index, int role) const {
  return QVariant();
}

TLEmuMemoryModel::TLEmuMemoryModel(AbstractEmulatorState *state) {
  this->state = state;
}
