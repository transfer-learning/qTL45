//
// Created by codetector on 12/5/19.
//

#include <fmt/format.h>
#include <iostream>
#include "TLEmuMemoryModel.h"

std::string TLEmuMemoryModel::formatMemoryValue(uint64_t addr) const {
  auto dataWidth = state->getMemoryDataWidth();

  switch(state->getMemoryMapping(addr)) {
    default:
      return "";
    case MemoryMapping::IO_MEMORY:
    case MemoryMapping::STANDARD_MEMORY: {
      uint64_t value = state->getMemoryValue(addr);

      if (dataWidth <= 4) {
        return fmt::format("0x{:04X}\t0x{:01X}\t", addr, value);
      } else if (dataWidth <= 8) {
        return fmt::format("0x{:04X}\t0x{:02X}\t", addr, value);
      } else if (dataWidth <= 16) {
        return fmt::format("0x{:04X}\t0x{:04X}\t", addr, value);
      } else if (dataWidth <= 32) {
        return fmt::format("0x{:04X}\t0x{:08X}\t", addr, value);
      } else {
        return fmt::format("0x{:04X}\t0x{:016X}\t", addr, value);
      }
    }
  }
}

void TLEmuMemoryModel::getDisplayLimits(uint64_t &min, uint64_t &max) const {
  min = 0;
  if (baseAddress > 1000) {
    min = baseAddress - 1000;
  }

  max = (1ULL << state->getMemoryAddrWidth()) - 1;
  if (max - 1000 > baseAddress) {
    max = baseAddress + 1000;
  }
}

int TLEmuMemoryModel::rowCount(const QModelIndex &parent) const {
  uint64_t min, max;
  getDisplayLimits(min, max);

  return (int) (max - min + 1);
}

QVariant TLEmuMemoryModel::data(const QModelIndex &index, int role) const {
  if (role == Qt::DisplayRole) {
    auto row = index.row();

    uint64_t min, max;
    getDisplayLimits(min, max);

    uint64_t addr = min + row;

    return QString::fromStdString(formatMemoryValue(addr));
  }
  return QVariant();
}