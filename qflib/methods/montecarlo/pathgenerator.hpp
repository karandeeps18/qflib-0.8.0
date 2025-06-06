/**
 * base of a path generator
 *  
 */





/**
@file  pathgenerator.hpp
@brief Base class for all Monte Carlo path generators
*/

#ifndef QF_PATHGENERATOR_HPP
#define QF_PATHGENERATOR_HPP


#include <qflib/defines.hpp>
#include <qflib/exception.hpp>
#include <qflib/sptr.hpp>

BEGIN_NAMESPACE(qf)

/** The abstract base class for all Monte Carlo path generators.
    It must be inherited by specific path generators.
*/
class PathGenerator
{
public:
  /** Dtor */
  virtual ~PathGenerator() {}

  /** Returns the number of time steps */
  size_t nTimeSteps() const;

  /** Returns the number of simulated factors */
  size_t nFactors() const;

  /** Returns the next price path.
      The Matrix is resized to size ntimesteps * nfactors
  */
  virtual void next(Matrix& pricePath) = 0;

protected:
  size_t ntimesteps_;    // the number of time steps
  size_t nfactors_;      // the number of factors
};

using SPtrPathGenerator = std::shared_ptr<PathGenerator>;

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline size_t PathGenerator::nTimeSteps() const
{
  return ntimesteps_;
}

inline size_t PathGenerator::nFactors() const
{
  return nfactors_;
}

END_NAMESPACE(qf)

#endif // QF_PATHGENERATOR_HPP
