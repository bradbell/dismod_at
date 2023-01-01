// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin cohort_ode_xam.cpp$$
$spell
   interp
   xam
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section C++ cohort_ode: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/cohort_ode.hpp>

namespace {
   using CppAD::vector;
   typedef CppAD::AD<double> Float;

   class Fun {
   public:
      void Ode(const Float& a, const vector<Float>& y, vector<Float>& yp)
      {
         Float iota  = 1.0 * a;
         Float rho   = 0.9 * a;
         Float chi   = 0.8 * a;
         Float omega = 0.7 * a;
         Float S       = y[0];
         Float C       = y[1];
         yp[0]         = - (iota + omega) * S + rho * C;
         yp[1]         = + iota * S - (rho + chi + omega) * C;
         return;
      }
   };
}

bool cohort_ode_xam(void)
{  bool   ok = true;
   size_t n  = 4;
   vector<double> age(n);
   vector<Float> iota(n), rho(n), chi(n), omega(n);
   //
   for(size_t k = 0; k < n; ++k)
   {  age[k] = double(k) / double(n-1);
      //
      // same function of age as in Ode above
      iota[k]  = 1.0 * age[k];
      rho[k]   = 0.9 * age[k];
      chi[k]   = 0.8 * age[k];
      omega[k] = 0.7 * age[k];
   }

   // call cohort_ode
   Float  pini   = 0.2;
   vector<Float> s_out(n), c_out(n);
   std::string rate_case = "iota_pos_rho_pos";
   dismod_at::cohort_ode(
      rate_case, age, pini, iota, rho, chi, omega, s_out, c_out
   );
   Fun F;
   size_t M = 100;
   Float ai = age[0];
   Float af = age[n-1];
   vector<Float> yi(2);
   yi[0] = 1.0 - pini;
   yi[1] = pini;
   vector<Float> yf = CppAD::Runge45(F, M, ai, af, yi);
   //
   // std::cout << 1.0 - s_out[n-1] / yf[0] << "\n";
   // std::cout << 1.0 - c_out[n-1] / yf[1] << "\n";
   //
   // very accurate because rates are linear w.r.t age
   ok &= fabs( 1.0 - s_out[n-1] / yf[0] ) < 1e-10;
   ok &= fabs( 1.0 - c_out[n-1] / yf[1] ) < 1e-10;
   //
   return ok;
}
// END C++
