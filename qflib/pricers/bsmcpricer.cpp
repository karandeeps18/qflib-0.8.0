/**
@file  bsmcpricer.cpp
@brief Implementation of the BsMcPricer class
*/

#include <qflib/pricers/bsmcpricer.hpp>
#include <cmath>

using namespace std;

BEGIN_NAMESPACE(qf)

// Constructor for constnt volatility
BsMcPricer::BsMcPricer(
    SPtrProduct prod,
    SPtrYieldCurve discountCurve,
    double divYield,
    double vol,           
    double spot,
    McParams mcparams)
: prod_(prod),
  discyc_(discountCurve),
  divyld_(divYield),
  vol_(vol),              // constant vol 
  volTS_(nullptr),        // no vol term structure
  spot_(spot),
  mcparams_(mcparams)
{

  Vector fixtimes = prod_->fixTimes();
  size_t ntimesteps = fixtimes.size();

  // Create the path generator 
  if (mcparams_.pathGenType == McParams::PathGenType::EULER) {
    if (mcparams_.urngType == McParams::UrngType::MINSTDRAND)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMinStdRand>(ntimesteps, 1));
    else if (mcparams_.urngType == McParams::UrngType::MT19937)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMt19937>(ntimesteps, 1));
    else if (mcparams_.urngType == McParams::UrngType::RANLUX3)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux3>(ntimesteps, 1));
    else if (mcparams_.urngType == McParams::UrngType::RANLUX4)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux4>(ntimesteps, 1));
    else
      QF_ASSERT(0, "BsMcPricer (constant vol): unknown URNG type!");
  }
  else
    QF_ASSERT(0, "BsMcPricer (constant vol): unknown path generator type!");

  // Pre-compute discount factors
  Vector const& paytimes = prod_->payTimes();
  discfactors_.resize(paytimes.size());
  for (size_t i = 0; i < paytimes.size(); ++i) {
    discfactors_[i] = discyc_->discount(paytimes[i]);
  }

  // Pre-compute stdevs and drifts from time step to time step
  drifts_.resize(ntimesteps);
  stdevs_.resize(ntimesteps);

  double t1 = 0.0;
  for (size_t i = 0; i < ntimesteps; ++i) {
    double t2 = fixtimes[i];
    double fwdrate = discyc_->fwdRate(t1, t2);

    double var = vol_ * vol_ * (t2 - t1); 
    stdevs_[i] = std::sqrt(var);
    drifts_[i] = (fwdrate - divyld_) * (t2 - t1) - 0.5 * var;

    t1 = t2; 
  }
  payamts_.resize(prod_->payTimes().size());
}


BsMcPricer::BsMcPricer(
    SPtrProduct prod,
    SPtrYieldCurve discountCurve,
    double divYield,
    SPtrVolatilityTermStructure volTS, // pointer to volatilityTermStructure
    double spot,
    McParams mcparams)
: prod_(prod),
  discyc_(discountCurve),
  divyld_(divYield),
  vol_(0.0),   
  volTS_(volTS),  // store pointer to volTS
  spot_(spot),
  mcparams_(mcparams)
{
  Vector fixtimes = prod_->fixTimes();
  size_t ntimesteps = fixtimes.size();

  // path generator
  if (mcparams_.pathGenType == McParams::PathGenType::EULER) {
    if (mcparams_.urngType == McParams::UrngType::MINSTDRAND)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMinStdRand>(ntimesteps, 1));
    else if (mcparams_.urngType == McParams::UrngType::MT19937)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngMt19937>(ntimesteps, 1));
    else if (mcparams_.urngType == McParams::UrngType::RANLUX3)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux3>(ntimesteps, 1));
    else if (mcparams_.urngType == McParams::UrngType::RANLUX4)
      pathgen_ = SPtrPathGenerator(new EulerPathGenerator<NormalRngRanLux4>(ntimesteps, 1));
    else
      QF_ASSERT(0, "BsMcPricer (term-structure vol): unknown URNG type!");
  }
  else
    QF_ASSERT(0, "BsMcPricer (term-structure vol): unknown path generator type!");

  // Pre-compute discount factors
  Vector const& paytimes = prod_->payTimes();
  discfactors_.resize(paytimes.size());
  for (size_t i = 0; i < paytimes.size(); ++i) {
    discfactors_[i] = discyc_->discount(paytimes[i]);
  }

  // Pre-compute stdevs and drifts from time step to time step
  drifts_.resize(ntimesteps);
  stdevs_.resize(ntimesteps);

  double t1 = 0.0;
  for (size_t i = 0; i < ntimesteps; ++i) {
    double t2 = fixtimes[i];
    double fwdrate = discyc_->fwdRate(t1, t2);

    double fwdVol = volTS_->fwdVol(t1, t2);
    double var = fwdVol * fwdVol * (t2 - t1);
    stdevs_[i] = std::sqrt(var);
    drifts_[i] = (fwdrate - divyld_) * (t2 - t1) - 0.5 * var;

    t1 = t2;
  }
  payamts_.resize(prod_->payTimes().size());
}


double BsMcPricer::processOnePath(Matrix& pricePath)
{
  // generate standard normal increments
  pathgen_->next(pricePath);
  double spot = spot_;
  for (size_t i = 0; i < pricePath.n_rows; ++i) {
    double normaldeviate = pricePath(i, 0);
    pricePath(i, 0) = spot * std::exp(drifts_[i] + stdevs_[i] * normaldeviate);
    spot = pricePath(i, 0);
  }


  prod_->eval(pricePath);
  payamts_ = prod_->payAmounts();
  double pv = 0.0;
  for (size_t i = 0; i < payamts_.size(); ++i) {
    pv += discfactors_[i] * payamts_[i];
  }

  return pv;
}

END_NAMESPACE(qf)
