# Mersenne Twister

Details on MT [here](http://en.wikipedia.org/wiki/Mersenne_Twister)

Info about fump ahead using Polynomial Multiplication method [here](http://www.math.sci.hiroshima-u.ac.jp/~m-mat/MT/JUMP/index.html)
and [here](http://www.math.sci.hiroshima-u.ac.jp/~m-MAT/MT/ARTICLES/mt.pdf)

## Usage

Before calling `jump_ahead` or `get_jump_aheaded_state` you need to initialize it by `init_jump`. Notice, that it will
force MT to increase it's state by 2 * k, where k = 19937 is Mersenne Twister exponent.

####Example here:

```
long jump_step = 1000000;
mersenne_twister hare{};
hare.init_jump(jump_step);

mersenne_twister tortoise{hare.get_state()};

hare.jump_ahead();

for(unsigned i = 0; i < jump_step; ++i)
{
  tortoise.get_random();
}

tortoise == hare;
```
