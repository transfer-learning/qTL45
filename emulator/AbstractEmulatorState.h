//
// Created by codetector on 12/3/19.
//

#ifndef TL45_EMU_ABSTRACTEMULATORSTATE_H
#define TL45_EMU_ABSTRACTEMULATORSTATE_H


#include <cstdint>
#include <string>

enum class MemoryMapping {
    NOT_MAPPED,
    STANDARD_MEMORY,
    IO_MEMORY
};

class AbstractEmulatorState {
public:
  virtual uint16_t getRegisterCount() = 0;
  virtual std::string getRegisterName(uint16_t regID) = 0;
  virtual bool hasRegisterAlias(uint16_t regID) = 0;
  virtual uint16_t getRegisterBitSize(uint16_t regID) = 0;

  virtual uint64_t getRegisterValue(uint64_t regID) = 0;
  virtual uint64_t getProgramCounterValue() = 0;

  virtual void step() = 0;
  virtual void run() = 0;
  virtual void clear() = 0;
  virtual int load(uint64_t, std::string) = 0;


  virtual unsigned getMemoryAddrWidth() = 0;
  virtual unsigned getMemoryDataWidth() = 0;

  virtual MemoryMapping getMemoryMapping(uint64_t addr) = 0;
  virtual void readMemory(uint64_t addr, size_t n, void* data) = 0;
  virtual void writeMemory(uint64_t addr, size_t n, void* data) = 0;
  virtual std::string getMemoryDisassembly(uint64_t &addr) = 0;

	virtual void* getRawMemoryPtr() const = 0;
};


#endif //TL45_EMU_ABSTRACTEMULATORSTATE_H
