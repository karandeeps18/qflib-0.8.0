/**
@file  market.cpp
@brief Implementation of the Market singleton and the market() free function
*/

#include <qflib/market/market.hpp>

BEGIN_NAMESPACE(qf)

Market& Market::instance()
{
  static Market theMarket;
  return theMarket;
}


void Market::clear()
{
  ycmap_.clear();
  volmap_.clear();
}

// The helper function
Market& market()
{
  return Market::instance();
}

END_NAMESPACE(qf)
