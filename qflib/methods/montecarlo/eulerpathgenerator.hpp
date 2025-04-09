/**
@file  pathgenerator.hpp
@brief Definition of Monte Carlo path generator with Euler time stepping
*/

#ifndef QF_EULERPATHGENERATOR_HPP
#define QF_EULERPATHGENERATOR_HPP

#include <qflib/methods/montecarlo/pathgenerator.hpp>
#include <qflib/math/random/rng.hpp>

BEGIN_NAMESPACE(qf)

/** Creates standard normal increments populating the time line sequentially.
    It is templetized on the underlying normal deviate generator.
*/
template <typename NRNG>
class EulerPathGenerator : public PathGenerator
{
public:

  /** Ctor for generating independent increments for independent factors */
  EulerPathGenerator(size_t ntimesteps, size_t nfactors);

  /** Returns the dimension of the generator */
  size_t dim() const;

  /** Returns the next price path */
  virtual void next(Matrix& pricePath) override;

protected:
  NRNG nrng_;
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

template <typename NRNG>
inline EulerPathGenerator<NRNG>::EulerPathGenerator(size_t ntimesteps, size_t nfactors)
  : nrng_(ntimesteps * nfactors, 0.0, 1.0)
{
  ntimesteps_ = ntimesteps;
  nfactors_ = nfactors;
}

template <typename NRNG>
inline size_t EulerPathGenerator<NRNG>::dim() const
{
  return nrng_.dim();
}

template <typename NRNG>
inline void EulerPathGenerator<NRNG>::next(Matrix& pricePath)
{
  pricePath.resize(ntimesteps_, nfactors_);
  // iterate over columns; the matrix will be filled column by column
  for (size_t j = 0; j < nfactors_; ++j) {
    Matrix::col_iterator cbegin = pricePath.begin_col(j);
    Matrix::col_iterator cend = pricePath.end_col(j);
    nrng_.next(cbegin, cend);
  }
}

END_NAMESPACE(qf)

#endif // QF_EULERPATHGENERATOR_HPP
