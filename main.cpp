#include <iostream>
#include "mersenne_twister.h"
#include <algorithm>
#include <vector>
#include <array>
#include <fstream>
#include <ctime>

using namespace std;

bool distribution_test()
{
  std::array<double, 1000000> rands{};
  mersenne_twister mrt{};

  for (int i = 0; i < 1000000; ++i)
    rands[i] = mrt.get_random();

  std::cout << "\nfor 1 000 000 iterations:" << std::endl;
  double mean = std::accumulate(rands.begin(), rands.end(), 0.)/rands.size();
  std::cout << "mean: " << mean << std::endl;

  std::transform(rands.begin(), rands.end(), rands.begin(), [&mean](double x){return x -= mean;});
  std::transform(rands.begin(), rands.end(), rands.begin(), [&mean](double x){return x*x;});

  double var = std::accumulate(rands.begin(), rands.end(), 0.)/rands.size();
  std::cout << "var: " << var << std::endl << std::endl;

  return !((abs(0.5000 - mean) > 0.001) || (0.0833 - var > 0.001));
}

void jump_ahead_timing()
{
  uint64_t jump_step = 3000000000;
  mersenne_twister hare{};

  double start = clock();
  std::cout << "\njump_ahead with step == 3 000 000 000:" << std::endl;
  std::cout << "\npreparations for jump_ahead ...." << std::endl;
  hare.init_jump(jump_step);
  std::cout << "completed in " << (clock() - start)/CLOCKS_PER_SEC
            << "sec." << " bet you haven't noticed (:" << std::endl;

  mersenne_twister tortoise{hare.get_state()};

  start = clock();
  std::cout << "\njump_ahead itself ...." << std::endl;
  hare.jump_ahead();
  std::cout << "completed in " << (clock() - start)/CLOCKS_PER_SEC
            << "sec." << " time flies!" << std::endl;

  start = clock();
  std::cout << "\ncomparing with traditional method ..." << std::endl;
  for(unsigned i = 0; i < jump_step; ++i)
  {
    tortoise.get_random();
    if (i == jump_step/6)
    {
      std::cout << "still going. wait a minute ..." << std::endl;
    }
    else if (i == 2*jump_step/6)
    {
      std::cout << "or two? ..." << std::endl;
    }
    else if (i == 3*jump_step/6)
    {
      std::cout << "i'm alive! dont kill me!! ..." << std::endl;
    }
    else if (i == 4*jump_step/6)
    {
      std::cout << "pleease? ..." << std::endl;
    }
    else if (i == 5*jump_step/6)
    {
      std::cout << "dont panic. it almost complete ..." << std::endl;
    }
  }
  if (tortoise != hare)
  {
    std::cout << "Ouch. Jump ahead appears to be incorrect. It's a secret. Don't tell anybody." << std::endl;
  }
  else
  {
    std::cout << "finished in " << (clock() - start)/CLOCKS_PER_SEC
             << "sec." << " sounds terrible ):" << std::endl;
  }
}

int main()
{
  cout << (distribution_test() ? "Distribution smoke test passed ^.^"
                              : "Something is wrong with distribution. Don't panic and call FBI.")
                                 << std::endl << std::endl;

  jump_ahead_timing();
}

