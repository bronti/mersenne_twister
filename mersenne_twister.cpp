#include "mersenne_twister.h"

#include <ctime>

using namespace internal;

mersenne_twister::mersenne_twister()
  : mersenne_twister(clock())
{
}

mersenne_twister::mersenne_twister(uint32_t seed)
  : shift(0)
  , jump_initialized(false)
  , jump_step(-1)
{
  init_seed(seed);
}

mersenne_twister::mersenne_twister(const array<uint32_t, MRT_N>& seed)
  : state(seed)
  , shift(0)
  , jump_initialized(false)
  , jump_step(-1)
{
}

bool mersenne_twister::operator==(const mersenne_twister& other) const
{
  if ((state[shift] & UPPER_MASK) != (other.state[other.shift] & UPPER_MASK))
  {
    return false;
  }
  for (int i = 1; i < MRT_N; ++i)
  {
    if (state[(shift + i) % MRT_N] != other.state[(other.shift + i) % MRT_N])
    {
      return false;
    }
  }
  return true;
}

bool mersenne_twister::operator!=(const mersenne_twister& other) const
{
  return !(*this == other);
}

array<uint32_t, MRT_N> mersenne_twister::get_state() const
{
  array<uint32_t, MRT_N> shifted_state{};
  copy(state.begin() + shift, state.end(), shifted_state.begin());
  copy(state.begin(), state.begin() + shift, shifted_state.begin() + MRT_N - shift);
  return shifted_state;
}

void mersenne_twister::init_seed(uint32_t seed)
{
  // I think it's terrible way to seed mst)), so
  // todo: do smth with it
  uint32_t m = (1 << 16) + 3;
  uint32_t current = seed;
  for (size_t i = 0; i < MRT_N; ++i) {
    state[i] = current;
    current = current * m + 1;
  }
}

uint32_t mersenne_twister::get_more()
{
  size_t next = (shift + 1) % MRT_N;

  uint32_t y = (state[shift] & UPPER_MASK) | (state[next] & LOWER_MASK);
  size_t m_th = (shift + MRT_M) % MRT_N;
  state[shift] = state[m_th] ^ (y >> 1) ^ (MRT_A * (y & LSB_MASK));

  uint32_t result = tempering(state[shift]);
  shift = next;
  return result;
}

uint32_t mersenne_twister::tempering(uint32_t stt)
{
  uint32_t y = stt;
  y ^= (y >> MRT_U);
  y ^= (y << MRT_S) & MRT_B;
  y ^= (y << MRT_T) & MRT_C;
  y ^= (y >> MRT_L);
  return y;
}

double mersenne_twister::get_random()
{
  return get_more() / (4. * (1 << 30) - 1);
}

uint32_t mersenne_twister::get_int(uint32_t min, uint32_t max)
{
  if (max <= min)
  {
    throw std::invalid_argument("incorrect interval");
  }
  double random = get_random();
  uint32_t result = (uint32_t)((max - min) * random + min);
  return (result >= max) ? max : result;
}


// precomputation of phi == minimal polinomial for matrix
// imp!!: computating takes 2 * MRT_K steps.
GF2X mersenne_twister::min_polynomial()
{
  vec_GF2 lin_gen_sequence(INIT_SIZE, 2 * MRT_K);

  // some kind of algebra magic here.
  // http://www.shoup.net/papers/fastirred.ps
  for (int i = 0; i < 2 * MRT_K; ++i)
  {
    get_more();  // increments state
    lin_gen_sequence[i] = get_output();      // "get_more" can be used instead of "get_output"
  }
  return MinPolySeq(lin_gen_sequence, MRT_K);
}

// precomputation of t^j mod phi
GF2X mersenne_twister::compute_g(uint64_t j)
{
  return PowerXMod(j, min_polynomial());
}

void mersenne_twister::init_jump(uint64_t j)
{
  precalc = compute_g(j);
  jump_initialized = true;
  jump_step = j;
}

long mersenne_twister::get_jump_step()
{
  return jump_step;
}


// jump ahead using Polynomial Multiplication method.
// two papers here:
// http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/JUMP/index.html
array<uint32_t, MRT_N> mersenne_twister::get_jump_aheaded_state()
{
  if (!jump_initialized)
  {
    throw std::invalid_argument("jump ahead isn't initialized");
  }
  GF2X big_g(INIT_SIZE, 2 * MRT_K - 1);
  GF2X h(INIT_SIZE, 3 * MRT_K - 2);

  for (int i = 2 * MRT_K - 2; i > -1; --i)
  {
    auto s = get_output();
    SetCoeff(big_g, i, s);
    get_more();                // increments state
  }

  h = precalc * big_g;

  std::bitset<MRT_K + 1> bits;
  bits[0] = 0;
  for (int i = 0; i < MRT_K; ++i)
  {
    bits[i + 1] = IsOne(coeff(h, 2 * MRT_K - 2 - i));
            // i + 1 because output func = LSB(x_1)
  }
  return recover_state(bits);
}

bool mersenne_twister::get_output() const
{
  return state[(shift + 1) % MRT_N] & LSB_MASK;
}

void mersenne_twister::jump_ahead()
{
  state = get_jump_aheaded_state();
  shift = 0;
}

// algorithm here in section 4.3
// http://www.math.sci.hiroshima-u.ac.jp/~m-MAT/MT/ARTICLES/mt.pdf
array<uint32_t, MRT_N> mersenne_twister::recover_state(const std::bitset<MRT_K + 1>& bits)
{
  array<uint32_t, MRT_K + 1> recover;
  recover[0] = 0;
  for (int i = 1; i <= MRT_K; ++i)
  {
    recover[i] = bits[i] & LSB_MASK;
  }
  for (int i = MRT_K; i > MRT_N - 1; --i)
  {
    uint32_t y = recover[i] ^ recover[i - MRT_N + MRT_M];
    if ((recover[i - MRT_N + 1] & LSB_MASK) != 0)
    {
      y ^= MRT_A;
    }
    y = (y << 1);
    y |= (LSB_MASK & bits[i - MRT_N + 1]);
    recover[i - MRT_N + 1] |= LOWER_MASK & y;
    recover[i - MRT_N] |= UPPER_MASK & y;
  }
  array<uint32_t, MRT_N> new_state{};
  copy(recover.begin(), recover.begin() + MRT_N, new_state.begin());
  return new_state;
}
