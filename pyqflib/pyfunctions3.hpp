#include <pyqflib/pyutils.hpp>
#include <qflib/defines.hpp>
#include <qflib/products/europeancallput.hpp>
#include <qflib/pricers/bsmcpricer.hpp>
#include <qflib/math/stats/meanvarcalculator.hpp>
#include <qflib/math/random/rng.hpp>
#include <qflib/exception.hpp>

using namespace std;

static
PyObject* pyQfEuroBSMC(PyObject* pyDummy, PyObject* pyArgs)
{
  PY_BEGIN;


  PyObject* pyPayoffType  = nullptr;
  PyObject* pyStrike      = nullptr;
  PyObject* pyTimeToExp   = nullptr;
  PyObject* pySpot        = nullptr;
  PyObject* pyDiscountCrv = nullptr;
  PyObject* pyDivYield    = nullptr;
  PyObject* pyVolatility  = nullptr;
  PyObject* pyMcParams    = nullptr;
  PyObject* pyNPaths      = nullptr;


  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOO",
        &pyPayoffType,
        &pyStrike,
        &pyTimeToExp,
        &pySpot,
        &pyDiscountCrv,
        &pyDivYield,
        &pyVolatility,
        &pyMcParams,
        &pyNPaths))
  {
    return nullptr;
  }


  int payoffType    = asInt(pyPayoffType);
  double strike     = asDouble(pyStrike);
  double timeToExp  = asDouble(pyTimeToExp);
  double spot       = asDouble(pySpot);
  std::string ycName = asString(pyDiscountCrv);
  qf::SPtrYieldCurve spyc = qf::market().yieldCurves().get(ycName);
  QF_ASSERT(spyc, "error: yield curve " + ycName + " not found");

  double divYield   = asDouble(pyDivYield);
  qf::McParams mcparams = asMcParams(pyMcParams);
  unsigned long npaths  = asInt(pyNPaths);
  qf::SPtrProduct spprod(new qf::EuropeanCallPut(payoffType, strike, timeToExp));

  // Check if pyVolatility is numeric or string
  bool isNumeric = PyFloat_Check(pyVolatility) || PyLong_Check(pyVolatility);
  bool isString  = PyUnicode_Check(pyVolatility) || PyBytes_Check(pyVolatility);
  std::unique_ptr<qf::BsMcPricer> pricer;

  if (isNumeric)
  {
    double vol = asDouble(pyVolatility);
    // BsMcPricer constructor that takes double for volatility
    pricer.reset(new qf::BsMcPricer(
        spprod,
        spyc,
        divYield,
        vol,
        spot,
        mcparams
    ));
  }
  else if (isString)
  {
    // volpath time dependent
    std::string volName = asString(pyVolatility);
    qf::SPtrVolatilityTermStructure spvts =
      qf::market().volatilities().get(volName);

    QF_ASSERT(spvts, "error: vol curve " + volName + " not found");

    // Call BsMcPricer constructor that takes SPtrVolatilityTermStructure
    pricer.reset(new qf::BsMcPricer(
        spprod,
        spyc,
        divYield,
        spvts,  // pass volatility curve pointer
        spot,
        mcparams
    ));
  }
  else
  {
    QF_ASSERT(false, "Invalid argument for volatility: must be numeric (float/int) or a string handle.");
    return nullptr;
  }


  qf::MeanVarCalculator<double*> sc(pricer->nVariables());
  pricer->simulate(sc, npaths);
  const qf::Matrix& results = sc.results();
  size_t nsamples  = sc.nSamples();
  double mean      = results(0, 0);
  double stderror  = results(1, 0);
  stderror         = std::sqrt(stderror / nsamples);

  // Build a Python dictionary to return
  PyObject* ret = PyDict_New();
  PyDict_SetItem(ret, asPyScalar("Mean"),   asPyScalar(mean));
  PyDict_SetItem(ret, asPyScalar("StdErr"), asPyScalar(stderror));

  return ret;

  PY_END;
}
