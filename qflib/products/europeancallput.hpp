/**
@file  europeancallput.hpp
@brief The payoff of a European Call/Put option
*/

#ifndef QF_EUROPEANCALLPUT_HPP
#define QF_EUROPEANCALLPUT_HPP

#include <qflib/products/product.hpp>

BEGIN_NAMESPACE(qf)

/** The European call/put class
*/
class EuropeanCallPut : public Product
{
public:
  /** Initializing ctor */
  EuropeanCallPut(int payoffType, double strike, double timeToExp);

  /** Evaluates the product given the passed-in path
      The "pricePath" matrix must have as many rows as
      the number of fixing times
  */
  virtual void eval(Matrix const& pricePath) override;

private:
  int payoffType_;     // 1: call; -1 put
  double strike_;
  double timeToExp_;
};

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
EuropeanCallPut::EuropeanCallPut(int payoffType, double strike, double timeToExp)
  : payoffType_(payoffType), strike_(strike), timeToExp_(timeToExp)
{
  QF_ASSERT(payoffType == 1 || payoffType == -1, "EuropeanCallPut: the payoff type must be 1 (call) or -1 (put)!");
  QF_ASSERT(strike > 0.0, "EuropeanCallPut: the strike must be positive!");
  QF_ASSERT(timeToExp > 0.0, "EuropeanCallPut: the time to expiration must be positive!");

  // only one fixing time, the expiration
  fixTimes_.resize(1);
  fixTimes_[0] = timeToExp_;

  // assume that it will settle (pay) at expiration
  // TODO allow payment time later than expiration
  payTimes_.resize(1);
  payTimes_[0] = timeToExp_;

  // this product generates only one payment
  payAmounts_.resize(1);
}

inline void EuropeanCallPut::eval(Matrix const& pricePath)
{
  double S_T = pricePath(0, 0);
  if (payoffType_ == 1)
    payAmounts_[0] = S_T >= strike_ ? S_T - strike_ : 0.0;
  else
    payAmounts_[0] = S_T >= strike_ ? 0.0 : strike_ - S_T;
}

END_NAMESPACE(qf)

#endif // QF_EUROPEANCALLPUT_HPP
