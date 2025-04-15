# QFGB8960: Advanced C++ for Finance
Fordham University  
Spring 2025

---

## Overview

This repository hosts the **qflib** C++ library for quantitative finance projects, including:
- **Monte Carlo pricing** (path generators, statistics calculators, pricers)
- **Market data structures** (yield curves, vol curves)
- **Products** such as European options and digital (cash-or-nothing) options
- **Python bindings** for easy usage and convergence analysis

Developed as part of **QFGB8960** at **Fordham University** (Spring 2025), the library demonstrates modern C++ design patterns, integration with numerical methods, and bridging to Python for prototyping and analysis.

## Key Features
- **BsMcPricer**: General Monte Carlo engine supporting constant or time-dependent volatility
- **EuropeanCallPut**: Standard payoff class for vanilla calls/puts
- **DigitalCallPut**: New payoff class for cash-or-nothing digital options
- **Excel/Python Integration**: Tools for price convergence graphs, scenario testing, etc.

## Build & Installation

```bash
git clone https://github.com/<YourUserName>/qflib.git
cd qflib
mkdir build && cd build
cmake ..
cmake --build .
