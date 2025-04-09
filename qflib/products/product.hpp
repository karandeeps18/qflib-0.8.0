/**
@file  product.hpp
@brief Base class for all financial products.
*/

#ifndef QF_PRODUCT_HPP
#define QF_PRODUCT_HPP

#include <qflib/defines.hpp>
#include <qflib/exception.hpp>
#include <qflib/sptr.hpp>
#include <qflib/math/matrix.hpp>

BEGIN_NAMESPACE(qf)

/** The abstract base class for all financial products.
    It must be inherited by specific product payoffs.
*/
class Product
{
public:
  /** Initializing ctor */
  explicit Product(std::string const& payccy = "USD");

  /** Dtor */
  virtual ~Product() {}

  /** Returns the fixing (observation) times */
  Vector const& fixTimes() const;

  /** Returns the payment times */
  Vector const& payTimes() const;

  /** Returns the payment amounts */
  Vector const& payAmounts() const;

  /** Evaluates the product given the passed-in path
      The "pricePath" matrix must have as many rows as the number of fixing times
  */
  virtual void eval(Matrix const& pricePath) = 0;

protected:
  std::string payccy_;
  Vector fixTimes_;       // the fixing (observation) times
  Vector payTimes_;       // the payment times
  Vector payAmounts_;     // the payment times
};

/** Smart pointer to Product */
using SPtrProduct = std::shared_ptr<Product>;

///////////////////////////////////////////////////////////////////////////////
// Inline definitions

inline
Product::Product(std::string const& payccy)
: payccy_(payccy)
{}

inline
Vector const& Product::fixTimes() const
{
  return fixTimes_;
}

inline
Vector const& Product::payTimes() const
{
  return payTimes_;
}

inline
Vector const& Product::payAmounts() const
{
  return payAmounts_;
}

END_NAMESPACE(qf)

#endif // QF_PRODUCT_HPP
