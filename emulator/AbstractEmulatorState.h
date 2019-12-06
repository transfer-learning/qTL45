//
// Created by codetector on 12/3/19.
//

#ifndef TL45_EMU_ABSTRACTEMULATORSTATE_H
#define TL45_EMU_ABSTRACTEMULATORSTATE_H


#include <cstdint>
#include <string>

class AbstractEmulatorState {
public:
  virtual uint16_t getRegisterCount() = 0;
  virtual std::string getRegisterName(uint16_t regID) = 0;
  virtual bool hasRegisterAlias(uint16_t regID) = 0;
  virtual uint16_t getRegisterBitSize(uint16_t regID) = 0;

  virtual uint64_t getRegisterValue(uint64_t regID) = 0;
  virtual uint64_t getProgramCounterValue() = 0;

  virtual uint64_t getMemoryMaxValue() = 0;
};


#endif //TL45_EMU_ABSTRACTEMULATORSTATE_H
