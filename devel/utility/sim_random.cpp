// $Id:$
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin sim_random dev}
{xrst_spell
   enum
   gsl
   rng
}

Simulate a Dismod_at Random Distribution
########################################

Syntax
******

| *z* = ``sim_random`` (
| |tab| ``density`` , ``mu`` , ``delta`` , ``eta`` , ``nu`` )

manage_gsl_rng
**************
The routine :ref:`manage_gsl_rng-name` sets up and controls the underlying
simulated random number generator.

density
*******
This argument has prototype

   ``density_enum`` *density*

It specifies the :ref:`density<get_density_table@density_enum>`
for the distribution that we are simulating.
The following table specifies the meaning of this choice:

.. list-table::
   :widths: auto

   * - ``uniform_enum``
     - not allowed
   * - ``gaussian_enum``
     - :ref:`statistic@Log-Density Function, D@Gaussian`
   * - ``cen_gaussian_enum``
     - :ref:`statistic@Log-Density Function, D@Censored Gaussian`
   * - ``log_gaussian_enum``
     - :ref:`statistic@Log-Density Function, D@Log-Gaussian`
   * - ``laplace_enum``
     - :ref:`statistic@Log-Density Function, D@Laplace`
   * - ``cen_laplace_enum``
     - :ref:`statistic@Log-Density Function, D@Censored Laplace`
   * - ``log_laplace_enum``
     - :ref:`statistic@Log-Density Function, D@Log-Laplace`
   * - ``students_enum``
     - :ref:`statistic@Log-Density Function, D@Student's-t`
   * - ``log_students_enum``
     - :ref:`statistic@Log-Density Function, D@Log-Student's-t`

mu
**
This argument has prototype

   ``double`` *mu*

In the case were *density* is
:ref:`density_table@Notation@Linear` ,
it is the mean for the distribution that we are simulating
(before possible censoring).
Otherwise ``log`` ( *mu* + *eta*  ) is the mean of the
log of the data we are simulating.

delta
*****
This argument has prototype

   ``double`` *delta*

It is assumed *delta* is greater than zero and not infinity.

Linear
======
In the case were *density* is linear,
*delta*
it is the standard deviation for *z* ``-`` *mu*
(before possible censoring).

Log
===
If the density is
:ref:`density_table@Notation@Log Scaled` ,
*delta* is the standard deviation for

   ``log`` ( *z* + *eta* ) ``- log`` ( *mu* + *eta*  )

eta
***
This argument has prototype

   ``double`` *eta*

In the case were *density* is
``log_gaussian_enum`` , ``log_laplace_enum``
or ``log_students_enum`` ,
this is the offset in the log transformation for the distribution.
In this case, it is assumed that *mu* + *eta*  > 0 .
Otherwise, *eta* is not used.

nu
**
This argument has prototype

   ``double`` *nu*

In the case were *density* is
``students_enum`` or ``log_students_enum`` , it is
the degrees of freedom in the Student's-t distribution.
Otherwise it is not used.

z
*
The return value has prototype

   ``double`` *z*

It simulates a sample from the specified distribution that is independent
for the any previous return values.
{xrst_toc_hidden
   example/devel/utility/sim_random_xam.cpp
}
Example
*******
The file :ref:`sim_random_xam.cpp-name` is an example and test of this simulation.

{xrst_end sim_random}
------------------------------------------------------------------------------
*/

# include <cmath>
# include <gsl/gsl_randist.h>
# include <dismod_at/sim_random.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE

double sim_random(
   density_enum density,
   double       mu     ,
   double       delta  ,
   double       eta    ,
   double nu           )
{  gsl_rng* rng = CppAD::mixed::get_gsl_rng();
   //
   assert( density != uniform_enum );
   assert( delta > 0.0 );
   // -----------------------------------------------------------------------
   // linear Gaussian
   if( density == gaussian_enum )
      return mu + gsl_ran_gaussian(rng, delta);
   if( density == cen_gaussian_enum )
      return std::max(0.0, mu + gsl_ran_gaussian(rng, delta) );
   //
   // linear Laplace
   if( density == laplace_enum )
   {  double width = delta / std::sqrt(2.0);
      return mu + gsl_ran_laplace(rng, width);
   }
   if( density == cen_laplace_enum )
   {  double width = delta / std::sqrt(2.0);
      return std::max(0.0, mu + gsl_ran_laplace(rng, width) );
   }
   //
   // linear students
   if( density == students_enum )
   {  assert( nu > 2.0 );
      double x = gsl_ran_tdist(rng, nu);
      return  mu +  x * std::sqrt( (nu - 2.0) / nu ) * delta;
   }
   // ----------------------------------------------------------------------
   // log transformed cases
   assert( mu + eta > 0.0 );
   //
   // difference from mean in transformed space
   double d_log;
   if( density == log_gaussian_enum || density == cen_log_gaussian_enum )
   {  // log Gaussian
      d_log = gsl_ran_gaussian(rng, delta);
   }
   else if( density == log_laplace_enum || density == cen_log_laplace_enum )
   {  // log Laplace
      double width = delta / std::sqrt(2.0);
      d_log = gsl_ran_laplace(rng, width);
   }
   else
   {  // log Students
      assert( density == log_students_enum );
      assert( nu > 2.0 );
      double x = gsl_ran_tdist(rng, nu);
      d_log = x * std::sqrt( (nu - 2.0) / nu ) * delta;
   }
   //
   // d_log = log(z + eta) - log(mu + eta)
   double z = std::exp( d_log ) * (mu + eta) - eta;
   //
   // log censored cases
   if( density == cen_log_gaussian_enum || density == cen_log_laplace_enum )
      z = std::max(0.0, z);
   //
   // log and not censored
   return z;
}

} // END_DISMOD_AT_NAMESPACE
