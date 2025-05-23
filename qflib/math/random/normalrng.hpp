/**
@file  normalrng.hpp
@brief Generator of random deviates from the Normal distribution
*/

#ifndef QF_NORMALRNG_HPP
#define QF_NORMALRNG_HPP


#include <qflib/defines.hpp>
#include <qflib/exception.hpp>
#include <random>

BEGIN_NAMESPACE(qf)

/** Generator of normal deviates. It is templatized on the underlying uniform RNG
*/
template<typename URNG>
class NormalRng
{

public:
  /** Ctor from distribution parameters */
  explicit NormalRng(size_t dimension, double mean = 0.0, double stdev = 1.0, URNG const & urng = URNG());

  /** Returns the dimension of the generator */
  size_t dim() const;

  /** Returns a batch of random deviates
      CAUTION: it requires end - begin == dimension() */
  template <typename ITER>
  void next(ITER begin, ITER end);

  /** Returns the underlying uniform rng. */
  URNG & urng();

private:

  // state
  size_t dim_;      // the dimension of the generator
  URNG urng_;       // the uniform random number generator
  std::normal_distribution<double> normcdf_;  // the normal distribution

};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template<typename URNG>
NormalRng<URNG>::NormalRng(size_t dimension, double mean, double stdev, URNG const & urng)
  : dim_(dimension), urng_(urng)
{
  QF_ASSERT(stdev > 0.0, "the standard deviation must be positive!");
  normcdf_ = std::normal_distribution<double>(mean, stdev);
}

template<typename URNG>
size_t NormalRng<URNG>::dim() const
{
  return dim_;
}

template<typename URNG>
template <typename ITER>
void NormalRng<URNG>::next(ITER begin, ITER end)
{
  for (ITER it = begin; it != end; ++it)
    *it = normcdf_(urng_);
}

template<typename URNG>
URNG & NormalRng<URNG>::urng()
{
  return urng_;
}

END_NAMESPACE(qf)

#endif // QF_NORMALRNG_HPP
