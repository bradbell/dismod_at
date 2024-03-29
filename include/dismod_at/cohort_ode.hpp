// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_COHORT_ODE_HPP
# define DISMOD_AT_COHORT_ODE_HPP

# include <cppad/cppad.hpp>

namespace dismod_at {

   template <class Float>
   extern void cohort_ode(
      const std::string&           rate_case ,
      const CppAD::vector<double>& age       ,
      const Float&                 pini      ,
      const CppAD::vector<Float>&  iota      ,
      const CppAD::vector<Float>&  rho       ,
      const CppAD::vector<Float>&  chi       ,
      const CppAD::vector<Float>&  omega     ,
            CppAD::vector<Float>&  s_out     ,
            CppAD::vector<Float>&  c_out
   );
}

# endif
