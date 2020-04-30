//
// Created by codetector on 11/23/19.
//

#include <unordered_set>
#include <unordered_map>

#ifndef TL45_EMU_TL45_EMU_CORE_H
#define TL45_EMU_TL45_EMU_CORE_H

#define INPUT_TAINT_START 0x100000
#define INPUT_TAINT_END 0x200000

class TL45EmulatorState;

namespace TL45 {

struct DecodedInstruction {
  uint32_t raw;
  int opcode;
  bool RI;
  bool LH;
  bool ZS;
  int DR;
  int SR1;
  int SR2;
  uint16_t raw_imm;

  static bool decode(uint32_t raw, DecodedInstruction &instr) {
    instr.raw = raw;
    instr.opcode = (raw >> (32U - 5U)) & 0x1FU;
    instr.RI = raw >> (32U - 6U) & 1U;
    instr.LH = raw >> (32U - 7U) & 1U;
    instr.ZS = raw >> (32U - 8U) & 1U;
    instr.DR = raw >> (32U - 12U) & 0xFU;
    instr.SR1 = raw >> (32U - 16U) & 0xFU;
    instr.SR2 = raw >> (32U - 20U) & 0xFU;
    instr.raw_imm = raw & 0xFFFFU;

    return true; // valid
  }

  uint32_t decode_imm() const {
    uint32_t imm = this->raw_imm;
    if (LH) {
      imm <<= 16u;
    } else if (ZS && ((imm >> 15u) & 1u)) {
      imm |= (0xFFFFu << 16u);
    }
    return imm;
  }

};

struct cell_taint {
  std::unordered_set<uint32_t> set;

  void clear() {
    set.clear();
  }

  cell_taint() {
  }
  
  cell_taint(const cell_taint& other) : set(other.set) {
  }

  cell_taint(const cell_taint&& other) : set(std::move(other.set)) {
  }

  cell_taint operator|(const cell_taint& other) const {
    cell_taint result(*this);
    result |= other;
    return result;
  }
  
  cell_taint& operator|=(const cell_taint& other) {
    set.insert(other.set.begin(), other.set.end());
    return *this;
  }

  cell_taint& operator=(const cell_taint& other) {
    set = other.set;
    return *this;
  }

  cell_taint& operator=(const cell_taint&& other) {
    set = std::move(other.set);
    return *this;
  }
};

template<typename T>
struct cell {
  T value;
  cell_taint taint;

  explicit cell<T>(T value) : value(value) {
  }

  cell<T>(T value, cell_taint taint) : value(value), taint(taint) {
  }

  template<typename U>
  cell<T>& operator+=(const cell<U>& rhs) {
    value += rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator-=(const cell<U>& rhs) {
    value -= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator*=(const cell<U>& rhs) {
    value *= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator/=(const cell<U>& rhs) {
    value /= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator&=(const cell<U>& rhs) {
    value &= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator^=(const cell<U>& rhs) {
    value ^= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator|=(const cell<U>& rhs) {
    value |= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator<<=(const cell<U>& rhs) {
    value <<= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T>& operator>>=(const cell<U>& rhs) {
    value >>= rhs.value;
    taint |= rhs.taint;
    return *this;
  }

  template<typename U>
  cell<T> operator+(const cell<U>& rhs) const {
    return cell<T>(value + rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator-(const cell<U>& rhs) const {
    return cell<T>(value - rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator*(const cell<U>& rhs) const {
    return cell<T>(value * rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator/(const cell<U>& rhs) const {
    return cell<T>(value / rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator&(const cell<U>& rhs) const {
    return cell<T>(value & rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator^(const cell<U>& rhs) const {
    return cell<T>(value ^ rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator|(const cell<U>& rhs) const {
    return cell<T>(value | rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator<<(const cell<U>& rhs) const {
    return cell<T>(value << rhs.value, taint | rhs.taint);
  }

  template<typename U>
  cell<T> operator>>(const cell<U>& rhs) const {
    return cell<T>(value >> rhs.value, taint | rhs.taint);
  }

  cell<T>& operator<<=(uint8_t scalar) {
    value <<= scalar;
    return *this;
  }

  cell<T>& operator>>=(uint8_t scalar) {
    value >>= scalar;
    return *this;
  }

  cell<T> operator<<(T scalar) const {
    return cell<T>(value << scalar, taint);
  }

  cell<T> operator>>(T scalar) const {
    return cell<T>(value >> scalar, taint);
  }

  cell<T> operator&(T scalar) const {
    return cell<T>(value & scalar, taint);
  }

  cell<T> operator~() const {
    return cell<T>(~value, taint);
  }

  template<typename U>
  operator cell<U>() const {
    return cell<U>((U) value, taint);
  }
};

typedef cell<uint32_t> regcell;

struct tl45_flags {
  uint8_t zf:1;
  uint8_t cf:1;
  uint8_t sf:1;
  uint8_t of:1;
};

typedef std::pair<uint32_t, bool> branch;

struct pair_hash
{
  template <class T1, class T2>
  std::size_t operator() (const std::pair<T1, T2> &pair) const
  {
    return std::hash<T1>()(pair.first) ^ std::hash<T2>()(pair.second);
  }
};

class tl45_state {
  uint32_t registers[15];
  uint8_t *memory;
  tl45_flags flags;

  struct {
    cell_taint registers[15];
    cell_taint flags;
    std::unordered_map<uint32_t, cell_taint> memory;
  } taint;

  friend class ::TL45EmulatorState;
public:
  struct {
    std::unordered_map<branch, int32_t, pair_hash> branch_count;
    std::unordered_map<branch, std::unordered_set<int32_t>, pair_hash> branch_taint;
  } profile;
  
  uint32_t pc;
  size_t max_memory_length; // must be multiple of 4

  void write_reg(unsigned reg, regcell cell) {
    if (reg != 0) {
      registers[reg - 1] = cell.value;
      taint.registers[reg - 1] = std::move(cell.taint);
    }
  }

  regcell read_reg(unsigned reg) const {
    if (reg == 0) {
      return regcell(0, cell_taint());
    }
    return regcell( registers[reg - 1], taint.registers[reg - 1] );
  }

  void write_flags(cell<tl45_flags> cell) {
    flags = cell.value;
    taint.flags = cell.taint;
  }
  
  cell<tl45_flags> read_flags() const {
    return cell<tl45_flags>(flags, taint.flags);
  }
  
  void write_byte(regcell dst, cell<uint8_t> cell) {
    uint32_t addr = dst.value;
    memory[addr] = cell.value;
    taint.memory[addr] = cell.taint | dst.taint;
    if (addr >= INPUT_TAINT_START && addr < INPUT_TAINT_END) {
      taint.memory[addr].set.insert(addr - INPUT_TAINT_START);
    }
  }

  cell<uint8_t> read_byte(regcell src) {
    uint32_t addr = src.value;
    if (addr >= INPUT_TAINT_START && addr < INPUT_TAINT_END) {
      taint.memory[addr].set.insert(addr - INPUT_TAINT_START);
    }
    return cell<uint8_t>{memory[addr], taint.memory[addr] | src.taint};
  }

  cell<uint16_t> read_halfword(regcell addr) {
    return read_byte(addr) << 8 | read_byte(addr + regcell(1));
  }

  void write_halfword(regcell addr, cell<uint16_t> cell) {
    write_byte(addr + regcell(0), (cell & 0x0000ff00) >> 8);
    write_byte(addr + regcell(1), (cell & 0x000000ff) >> 0);
  }
  
  regcell read_word(regcell addr) {
    regcell result(0);
    result |= read_byte(addr);
    result <<= 8;
    result |= read_byte(addr + regcell(1));
    result <<= 8;
    result |= read_byte(addr + regcell(2));
    result <<= 8;
    result |= read_byte(addr + regcell(3));
    return result;
  }

  void write_word(regcell addr, regcell cell) {
    write_byte(addr + regcell(0), (cell & 0xff000000) >> 24);
    write_byte(addr + regcell(1), (cell & 0x00ff0000) >> 16);
    write_byte(addr + regcell(2), (cell & 0x0000ff00) >> 8);
    write_byte(addr + regcell(3), (cell & 0x000000ff) >> 0);
  }

  uint32_t fetch_instruction(uint32_t location) {
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
      value = (value << 8U) | memory[location + i];
    }
    return value;
  }
};

void tick(tl45_state *state);

}
  
#endif //TL45_EMU_TL45_EMU_CORE_H
