// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin eigen_ode2_xam.cpp$$
$spell
   eigen
$$

$section Example and Test of Eigenvector Method of Solving ODE$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$
$end
*/
// BEGIN C++
# include <cppad/cppad.hpp>
# include <dismod_at/eigen_ode2.hpp>

namespace {
   typedef CppAD::AD<double> Float;
   class Fun {
      typedef CppAD::vector<Float> vector;
   private:
      vector b_;
   public:
      void Ode(const Float& t, const vector& y, vector& yp)
      {  yp[0] = b_[0] * y[0] + b_[1] * y[1];
         yp[1] = b_[2] * y[0] + b_[3] * y[1];
         return;
      }
      void set(const vector& b)
      {  b_.resize( b.size() );
         b_ = b;
      }
   };
}

bool eigen_ode2_xam(void)
{  bool ok = true;
   typedef CppAD::vector<Float> vector;
   Float tf = 0.1;
   vector b(4), yi(2), yf(2);
   size_t case_number;
   // -------------------------------------------------------------------
   case_number = 1;
   //
   // b1 = 0, b2 = 0
   b[0] = -3.0;  b[1] = 0.0;
   b[2] = 0.0;   b[3] = -4.0;
   yi[0] = 1.0;
   yi[1] = 2.0;
   //
   // solve ODE using Runge45
   Fun F;
   F.set(b);
   size_t M  = 20;
   Float ti = 0.0;
   vector xi = yi;
   vector xf = CppAD::Runge45(F, M, ti, tf, yi);
   //
   yf  = dismod_at::eigen_ode2(case_number, b, yi, tf);
   ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
   ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
   // -------------------------------------------------------------------
   case_number = 2;
   //
   // b1 != 0, b2 = 0, b0 != b3
   b[0] = -3.0 , b[1] =  1.0;
   b[2] =  0.0 , b[3] = -4.0;
   F.set(b);
   xf = CppAD::Runge45(F, M, ti, tf, yi);
   yf  = dismod_at::eigen_ode2(case_number, b, yi, tf);
   ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
   ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
   //
   // b1 != 0, b2 = 0, b0 == b3
   b[0] = b[3];
   F.set(b);
   xf = CppAD::Runge45(F, M, ti, tf, yi);
   yf  = dismod_at::eigen_ode2(case_number, b, yi, tf);
   ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
   ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
   // -------------------------------------------------------------------
   case_number = 3;
   //
   // b1 = 0, b2 != 0, b0 != b3
   b[0] = -3.0 , b[1] =  0.0;
   b[2] =  1.0 , b[3] = -4.0;
   F.set(b);
   xf = CppAD::Runge45(F, M, ti, tf, yi);
   yf   = dismod_at::eigen_ode2(case_number, b, yi, tf);
   ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
   ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
   //
   // b1 = 0, b2 != 0, b0 == b3
   b[0] = b[3];
   F.set(b);
   xf = CppAD::Runge45(F, M, ti, tf, yi);
   yf   = dismod_at::eigen_ode2(case_number, b, yi, tf);
   ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
   ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
   // -------------------------------------------------------------------
   case_number = 4;
   //
   // b1 != 0, b2 != 0
   b[0] = -3.0 , b[1] =  1.0;
   b[2] =  1.0 , b[3] = -3.0;
   F.set(b);
   xf = CppAD::Runge45(F, M, ti, tf, yi);
   yf   = dismod_at::eigen_ode2(case_number, b, yi, tf);
   ok &= fabs( yf[0] / xf[0] - 1.0 ) < 1e-6;
   ok &= fabs( yf[1] / xf[1] - 1.0 ) < 1e-6;
   // -------------------------------------------------------------------
   return ok;
}
// END C++
