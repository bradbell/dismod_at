// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin residual_density dev}
{xrst_spell
   enum
   fabs
   logden
   wres
}

Compute Weighted Residual and Log-Density
#########################################

Prototype
*********
{xrst_literal
   // BEGIN_RESIDUAL_DENSITY
   // END_RESIDUAL_DENSITY
}

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

residual_type
*************

.. csv-table::
   :widths: auto

   ``simulated_data_enum`` , residual for the simulated data *z*
   ``real_data_enum`` , residual for the real data *y*
   ``value_prior_enum`` , residual for the variable value *y*
   ``difference_prior_enum`` , residual for the variable difference *z* - *y*

The difference prior cannot be used with a censored density; see
*d_enum* below.

z
*
is either simulated data, first variable in a variable difference, or nan;
see *residual_type* cases above.

y
*
is either the real data, the variable in a value prior,
second variable in a variable difference prior, or nan;
see *residual_type* cases above.

mu
**
is the mean value for the residual.

delta
*****
It is the standard deviation for the residual.
For log data densities it is in log space.
For all other cases, linear densities; i.e., it is in the same space as *mu* .

d_enum
******
Is the density for this residual.
If this is a prior residual, *d_enum* is the corresponding
:ref:`prior_table@density_id` in a prior table.
Otherwise, it is the corresponding
:ref:`data_table@density_id` in a data table.

d_eta
*****
If the density is :ref:`density_table@Notation@Log Scaled` ,
*eta* specifies the offset in the log transformation.
Otherwise it is not used.

d_nu
****
If the density *d_enum* is
``students_enum``  or ``log_students_enum`` ,
*nu* specifies the degrees of freedom in the Students-t distribution.
Otherwise it is not used.

index
*****
This is an identifying index for the residual.
It is only copied to the output structure and is not used in any other way.

residual
********

residual_struct
===============
This structure has the following fields:

.. list-table::
   :widths: auto

   * - Type
     - Field
     - Description
   * - *Float*
     - ``wres``
     - :ref:`weighted residual<statistic@Weighted Residual Function, R>`
   * - *Float*
     - ``logden_smooth``
     - this smooth term is in
       :ref:`log-density<statistic@Log-Density Function, D>`
   * - *Float*
     - ``logden_sub_abs``
     - absolute value of this smooth term is in log-density
   * - ``density_enum``
     - ``density``
     - type of density function; see
       :ref:`get_density_table@density_enum`
   * - ``size_t``
     - ``index``
     - identifier for this residual; see
       :ref:`residual_density@index` above.

wres
====
If this is a real data residual or a value prior residual::

   wres = R(y, mu, delta, d)

If this is a simulated data residual::

   wres = R(z, mu, delta, d)

If this is a difference prior residual::

   wres = R(z, y, mu, delta, d)

See :ref:`statistic@Weighted Residual Function, R` .

Log Density
===========
If this is a difference residual, the log-density function is

   *logden_smooth* - fabs( *logden_sub_abs* ))

see :ref:`log-density function<statistic@Log-Density Function, D>` .
Both *logden_smooth* and *logden_sub_abs*
are smooth functions of *mu* and *delta* .
This expresses the log-density
in terms of smooth functions (for optimization purposes).

Absolute Value Terms
====================
If *logden_sub_abs* is a CppAD constant,
(not a dynamic parameter or variable),
there is no absolute value term for this residual.

{xrst_end residual_density}
*/
# include <cppad/cppad.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a1_double.hpp>

namespace {
   template <class Float>
   void print_forward_if_not_positive(
      const char* name    ,
      const Float& value  )
   {  std::string lable = "residual_density: ";
      lable += name;
      lable += " = ";
      CppAD::PrintFor(value, lable.c_str(), value, "\n");
   }
   void print_forward_if_not_positive(
      const char* name    ,
      const double& value )
   {  assert( value > 0.0 );
   }

   template <class Float>
   Float log_n_choose_k(const Float& n, const Float& k)
   {  // https://en.wikipedia.org/wiki/Stirling%27s_approximation
      //
      // approximately log( n! )
      Float log_n_fac = n * log(n) - n + 1.0;
      //
      // approximately log( (n-k) ! )
      Float log_n_k_fac = (n-k) * log(n - k) - (n - k) + 1.0;
      //
      // approximately log( k! )
      Float log_k_fac = k * log(k) - k + 1.0;
      //
      // approximately (n/k)
      return log_n_fac - log_n_k_fac - log_k_fac;
   }
}


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// BEGIN_RESIDUAL_DENSITY
// residual =
template <class Float>
residual_struct<Float> residual_density(
   residual_enum      residual_type  ,
   const Float&       z              ,
   const Float&       y              ,
   const Float&       mu             ,
   const Float&       delta          ,
   density_enum       d_enum         ,
   const Float&       d_eta          ,
   const Float&       d_nu           ,
   const Float&       d_sample_size  ,
   size_t             index          )
// END_RESIDUAL_DENSITY
{

# ifndef NDEBUG
   switch( residual_type )
   {
      default:
      assert( false );
      break;

      case simulated_data_enum:
      assert( CppAD::isnan(y) );
      assert( ! CppAD::isnan( z ) );
      break;

      case real_data_enum:
      assert( ! CppAD::isnan(y) );
      assert( CppAD::isnan( z ) );
      break;

      case value_prior_enum:
      assert( ! CppAD::isnan(y) );
      assert( CppAD::isnan( z ) );
      assert( d_enum != binomial_enum );
      break;

      case difference_prior_enum:
      assert( ! CppAD::isnan(y) );
      assert( ! CppAD::isnan( z ) );
      assert( ! censored_density(d_enum) );
      assert( d_enum != binomial_enum );
      break;
   }
# endif

   // nan, r2, pi
   double nan = std::numeric_limits<double>::quiet_NaN();
   double r2  = std::sqrt(2.0);
   double pi2 = 8.0 * std::atan(1.0);

   // wres
   Float wres  = nan;
   switch( d_enum )
   {
      default:
      assert( false );
      break;


      case uniform_enum:
      wres = 0.0;
      break;

      case gaussian_enum:
      case cen_gaussian_enum:
      case laplace_enum:
      case cen_laplace_enum:
      case students_enum:
      case binomial_enum:
      assert( delta > 0.0 );
      print_forward_if_not_positive("delta", delta);
      switch( residual_type )
      {  default:
         assert(false);
         break;

         case real_data_enum:
         case value_prior_enum:
         wres = (y - mu) / delta;
         break;

         case simulated_data_enum:
         wres = (z - mu) / delta;
         break;

         case difference_prior_enum:
         wres  = (z - y - mu) / delta;
         break;
      }
      break;

      case log_gaussian_enum:
      case log_laplace_enum:
      case log_students_enum:
      case cen_log_gaussian_enum:
      case cen_log_laplace_enum:
      assert( delta > 0.0 );
      print_forward_if_not_positive("delta", delta);
      switch( residual_type )
      {  default:
         assert(false);
         break;

         case real_data_enum:
         case value_prior_enum:
         wres  = ( log( y + d_eta ) - log( mu + d_eta ) ) / delta;
         break;

         case simulated_data_enum:
         wres  = ( log( z + d_eta ) - log( mu + d_eta ) ) / delta;
         break;

         case difference_prior_enum:
         wres  = ( log( z + d_eta ) - log( y + d_eta ) - mu ) / delta;
         break;
      }
   }

   // censor
   bool  censor = false;
   if( censored_density(d_enum) )
   {  if( residual_type == simulated_data_enum )
         censor = z <= 0;
      else
      {  assert( residual_type != difference_prior_enum );
         censor = y <= 0;
      }
   }

   // logden_smooth, log_den_sub_abs
   Float logden_smooth  = nan;
   Float logden_sub_abs = 0.0;
   switch( d_enum )
   {
      default:
      assert(false);
      break;

      case uniform_enum:
      logden_smooth  = 0.0;
      break;

      case binomial_enum:
      {  // k = B(n, p), y = k / n
         Float n = d_sample_size;
         Float p = mu;
         Float k = n * y;
         logden_smooth = log_n_choose_k(n, k) + log(p) * k + (1.0-p) * (n-k);
      }

      case gaussian_enum:
      case log_gaussian_enum:
      logden_smooth  = - log( delta * sqrt( pi2 ) ) - wres * wres / 2.0;
      break;

      case laplace_enum:
      case log_laplace_enum:
      logden_smooth  = - log( delta * r2 );
      logden_sub_abs = r2 * wres;
      break;

      case students_enum:
      case log_students_enum:
      {  Float  r       = 1.0 + wres * wres / ( d_nu - 2.0 );
         logden_smooth  =  - log( r ) * (d_nu + 1.0) / 2.0;
      }
      break;

      case cen_gaussian_enum:
      if( censor )
      {  Float c = 0.0;
         Float erfc_value = CppAD::erfc( (mu - c) / ( delta * r2 ) );
         logden_smooth    = log(erfc_value / 2.0 );
      }
      else
      {  logden_smooth  = - log( delta * sqrt( pi2 ) ) - wres * wres / 2.0;
      }
      break;

      case cen_log_gaussian_enum:
      if( censor )
      {  Float c = 0.0;
         Float erfc_value =
            CppAD::erfc( (log(mu + d_eta) - log(c + d_eta)) / ( delta * r2 ) );
         logden_smooth    = log(erfc_value / 2.0 );
      }
      else
      {  logden_smooth  = - log( delta * sqrt( pi2 ) ) - wres * wres / 2.0;
      }
      break;

      case cen_laplace_enum:
      if( censor )
      {  Float c = 0.0;
         logden_smooth = - (mu - c) * r2 / delta - std::log(2.0);
      }
      else
      {  logden_smooth  = - log( delta * r2 );
         logden_sub_abs = r2 * wres;
      }
      break;

      case cen_log_laplace_enum:
      if( censor )
      {  Float c = 0.0;
         logden_smooth =
            - (log(mu + d_eta) - log(c + d_eta)) * r2 / delta - std::log(2.0);
      }
      else
      {  logden_smooth  = - log( delta * r2 );
         logden_sub_abs = r2 * wres;
      }
      break;
   }
   //
   residual_struct<Float> residual;
   residual.wres           = wres;
   residual.logden_smooth  = logden_smooth;
   residual.logden_sub_abs = logden_sub_abs;
   residual.density        = d_enum;
   residual.index          = index;
   return residual;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(Float)      \
   template residual_struct<Float> residual_density(        \
      residual_enum      residual_type    ,                 \
      const Float&       z                ,                 \
      const Float&       y                ,                 \
      const Float&       mu               ,                 \
      const Float&       delta            ,                 \
      density_enum       d_enum           ,                 \
      const Float&       d_eta            ,                 \
      const Float&       d_nu             ,                 \
      const Float&       d_sample_size    ,                 \
      size_t             index                              \
   );

// instantiations
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( double )
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( a1_double )

} // END DISMOD_AT_NAMESPACE
