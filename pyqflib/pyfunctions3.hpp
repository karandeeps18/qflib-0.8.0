/**
@file  pyfunctions3.hpp
@brief Implementation of Python callable functions
*/
#include <pyqflib/pyutils.hpp>

#include <qflib/defines.hpp>
#include <qflib/products/europeancallput.hpp>
#include <qflib/pricers/bsmcpricer.hpp>
#include <qflib/math/stats/meanvarcalculator.hpp>
#include <qflib/math/random/rng.hpp>

using namespace std;

static
PyObject*  pyQfEuroBSMC(PyObject* pyDummy, PyObject* pyArgs)
{
PY_BEGIN;

  PyObject* pyPayoffType(NULL);
  PyObject* pySpot(NULL);
  PyObject* pyStrike(NULL);
  PyObject* pyTimeToExp(NULL);
  PyObject* pyDiscountCrv(NULL);
  PyObject* pyDivYield(NULL);
  PyObject* pyVolatility(NULL);
  PyObject* pyMcParams(NULL);
  PyObject* pyNPaths(NULL);

  if (!PyArg_ParseTuple(pyArgs, "OOOOOOOOO", &pyPayoffType, &pyStrike, &pyTimeToExp, 
    &pySpot, &pyDiscountCrv, &pyDivYield, &pyVolatility, &pyMcParams, &pyNPaths))
    return NULL;

  int payoffType = asInt(pyPayoffType);
  double spot = asDouble(pySpot);
  double strike = asDouble(pyStrike);
  double timeToExp = asDouble(pyTimeToExp);

  std::string name = asString(pyDiscountCrv);
  qf::SPtrYieldCurve spyc = qf::market().yieldCurves().get(name);
  QF_ASSERT(spyc, "error: yield curve " + name + " not found");

  double divYield = asDouble(pyDivYield);
  double vol = asDouble(pyVolatility);
 
  // read the MC parameters
  qf::McParams mcparams = asMcParams(pyMcParams);
  // read the number of paths
  unsigned long npaths = asInt(pyNPaths);

  // create the product
  qf::SPtrProduct spprod(new qf::EuropeanCallPut(payoffType, strike, timeToExp));
  // create the pricer
  qf::BsMcPricer bsmcpricer(spprod, spyc, divYield, vol, spot, mcparams);
  // create the statistics calculator
  qf::MeanVarCalculator<double *> sc(bsmcpricer.nVariables());
  // run the simulation
  bsmcpricer.simulate(sc, npaths);
  // collect results
  qf::Matrix const& results = sc.results();
  // read out results
  size_t nsamples = sc.nSamples();
  double mean = results(0, 0);
  double stderror = results(1, 0);
  stderror = std::sqrt(stderror / nsamples);

  // write mean and standard error into a Python dictionary
  PyObject* ret = PyDict_New();
  int ok = PyDict_SetItem(ret, asPyScalar("Mean"), asPyScalar(mean));
  PyDict_SetItem(ret, asPyScalar("StdErr"), asPyScalar(stderror));
  return ret;

PY_END;
}
