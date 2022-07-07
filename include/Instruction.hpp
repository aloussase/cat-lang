#pragma once

#include <string>

namespace cat
{

class Instruction
{
public:
  virtual ~Instruction(){};

  virtual std::string to_s() const noexcept = 0;

  class LI;
  class MOVE;
  class ADD;
  class SUB;
  class SUBU;
  class MULT;
  class MFLO;
  class ADDI;
  class LW;
  class SW;
  class SLT;
  class SLTU;
  class XORI;
};

class Instruction::LI : public Instruction
{
public:
  LI(const std::string& reg, int value) : reg_{ reg }, value_{ value } {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "li    " + reg_ + ", " + std::to_string(value_);
  }

private:
  std::string reg_;
  int value_;
};

class Instruction::MOVE : public Instruction
{
public:
  MOVE(const std::string& r1, const std::string& r2) : r1_{ r1 }, r2_{ r2 } {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "move  " + r1_ + ", " + r2_;
  }

private:
  std::string r1_;
  std::string r2_;
};

class Instruction::MFLO : public Instruction
{
public:
  MFLO(const std::string& r) : r_{ r } {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "mflo  " + r_;
  }

private:
  std::string r_;
};

class Instruction::ADD : public Instruction
{
public:
  ADD(const std::string& rd, const std::string& rs, const std::string& rt) : rd_{ rd }, rs_{ rs }, rt_{ rt }
  {
  }

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "add   " + rd_ + ", " + rs_ + ", " + rt_;
  }

private:
  std::string rd_;
  std::string rs_;
  std::string rt_;
};

class Instruction::SUB : public Instruction
{
public:
  SUB(const std::string& rd, const std::string& rs, const std::string& rt) : rd_{ rd }, rs_{ rs }, rt_{ rt }
  {
  }

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "sub   " + rd_ + ", " + rs_ + ", " + rt_;
  }

private:
  std::string rd_;
  std::string rs_;
  std::string rt_;
};

class Instruction::SUBU : public Instruction
{
public:
  SUBU(const std::string& rd, const std::string& rs, const std::string& rt) : rd_{ rd }, rs_{ rs }, rt_{ rt }
  {
  }

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "subu   " + rd_ + ", " + rs_ + ", " + rt_;
  }

private:
  std::string rd_;
  std::string rs_;
  std::string rt_;
};

class Instruction::MULT : public Instruction
{
public:
  MULT(const std::string& r1, const std::string& r2) : r1_{ r1 }, r2_{ r2 } {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "mult  " + r1_ + ", " + r2_;
  }

private:
  std::string r1_;
  std::string r2_;
};

class Instruction::ADDI : public Instruction
{
public:
  ADDI(const std::string& rd, const std::string& rs, int constant)
      : rd_{ rd }, rs_{ rs }, constant_{ constant }
  {
  }

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "addi  " + rd_ + ", " + rs_ + ", " + std::to_string(constant_);
  }

private:
  std::string rd_;
  std::string rs_;
  int constant_;
};

class Instruction::LW : public Instruction
{
public:
  LW(const std::string& rs, int offset, const std::string& rd) : rs_{ rs }, rd_{ rd }, offset_{ offset } {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "lw    " + rs_ + ", " + std::to_string(offset_) + "(" + rd_ + ")";
  }

private:
  std::string rs_ = {};
  std::string rd_ = {};
  int offset_ = {};
};

class Instruction::SW : public Instruction
{
public:
  SW(const std::string& rs, int offset, const std::string& rd) : rs_{ rs }, rd_{ rd }, offset_{ offset } {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "sw    " + rs_ + ", " + std::to_string(offset_) + "(" + rd_ + ")";
  }

private:
  std::string rs_ = {};
  std::string rd_ = {};
  int offset_ = {};
};

class Instruction::SLT : public Instruction
{
public:
  SLT(const std::string& rd, const std::string& rs, const std::string& rt)
      : m_rd{ rd }, m_rs{ rs }, m_rt{ rt }
  {
  }

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "slt    " + m_rd + ", " + m_rs + ", " + m_rt;
  }

private:
  std::string m_rd = {};
  std::string m_rs = {};
  std::string m_rt = {};
};

class Instruction::SLTU : public Instruction
{
public:
  SLTU(const std::string& rd, const std::string& rs, const std::string& rt)
      : m_rd{ rd }, m_rs{ rs }, m_rt{ rt }
  {
  }

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "sltu    " + m_rd + ", " + m_rs + ", " + m_rt;
  }

private:
  std::string m_rd = {};
  std::string m_rs = {};
  std::string m_rt = {};
};

class Instruction::XORI : public Instruction
{
public:
  XORI(const std::string& rt, const std::string& rs, uint32_t immediate):
    m_rt{rt}, m_rs{rs}, m_immediate{immediate}
  {}

  [[nodiscard]] std::string
  to_s() const noexcept override
  {
    return "xori    " + m_rt + ", " + m_rs + ", " + std::to_string(m_immediate);
  }

private:
  std::string m_rt = {};
  std::string m_rs = {};
  uint32_t m_immediate = {};
};

}
