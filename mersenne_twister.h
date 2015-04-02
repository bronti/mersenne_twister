#pragma once

#include <stdint.h>
#include <stdlib.h>
#include <bitset>
#include <array>

#include "NTL/GF2X.h"


// to escape importing of namespaces in NTL_CLIENT
namespace internal
{

NTL_CLIENT

class mersenne_twister{

#define MRT_W  32

#define MRT_K  19937    // used for jump ahead

#define MRT_N  624
#define MRT_R  31
#define MRT_M  397
#define MRT_U  11
#define MRT_S  7
#define MRT_T  15
#define MRT_L  18
#define MRT_A  0x9908B0DF
#define MRT_B  0x9D2C5680
#define MRT_C  0xEFC60000

private:

  static const uint32_t ALL_MASK   = 0xFFFFFFFF;
  static const uint32_t UPPER_MASK = ALL_MASK << MRT_R;
  static const uint32_t LOWER_MASK = ALL_MASK ^ UPPER_MASK;
  static const uint32_t LSB_MASK   = 0x01;                     // for jump ahead

  array<uint32_t, MRT_N> state;
  int shift;                     // first element's index

  mutable NTL::GF2X precalc;
  mutable bool      jump_initialized;
  mutable long      jump_step;


  void init_seed(uint32_t seed);
  uint32_t get_more();
  static uint32_t tempering(uint32_t stt);

  NTL::GF2X min_polynomial();
  GF2X compute_g(uint64_t j);
  bool get_output() const;           //o function for jump ahead
  array<uint32_t, MRT_N> recover_state(const std::bitset<MRT_K + 1>& bits);

public:
  mersenne_twister();
  mersenne_twister(uint32_t seed);
  mersenne_twister(std::bitset<MRT_K> seed);
  mersenne_twister(const array<uint32_t, MRT_N>& seed);
  //todo: move
  double   get_random();
  uint32_t get_int(uint32_t min, uint32_t max);

  array<uint32_t, MRT_N> get_state() const;
  bool operator==(const mersenne_twister& other) const;
  bool operator!=(const mersenne_twister& other) const;

  void init_jump(uint64_t  j);
  long get_jump_step();
  array<uint32_t, MRT_N> get_jump_aheaded_state();
  void jump_ahead();
};

} //namespace internal;

using internal::mersenne_twister;
