// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin residual_density}
{xrst_spell
   bool
   enum
   fabs
   logden
   wres
}

Compute Weighted Residual and Log-Density
#########################################

Syntax
******

| *residual* = ``residual_density`` (
| |tab| *z* , *y* , *mu* , *delta* , *d_id* , *d_eta* , *d_nu* , *index* , *diff* , *prior*
| )

Float
*****
The type *Float* must be ``double`` or
:ref:`a1_double-name` .

z
*
This argument has prototype

   ``const`` *Float* & ``z``

#. If *diff* is true, *z* is not nan and *prior* is true.
   In this case *z* specifies the first random variable in the difference.
#. If *diff* is false and *z* is not nan,
   *prior* must be false and
   *z* is a simulated value for the measurement *y* .
#. If *diff* is false and *z* is nan,
   the likelihood is for the single random variable *y* .

y
*
This argument has prototype

   ``const`` *Float* & ``y``

If *diff* is true,
*y* specifies the second random variable in the difference.
If *diff* is false and *z* is nan,
*y* is used both for measurement value
and for calculating log-density standard deviations.
If *diff* is false and *z* is not nan,
*y* is only used for calculating log-density standard deviations.

mu
**
This argument has prototype

   ``const`` *Float* & ``mu``

If *diff* is true, it is the central value for the difference.
Otherwise, it is the central value for *y* .

delta
*****
This argument has prototype

   ``const`` *Float* & ``delta``

It is either the standard deviation.
For log data densities it is in log space.
For all other cases (linear data densities or priors) it is in the
same space as *y* .

d_id
****
This argument has prototype

   ``density_enum`` *d_id*

If *prior* is true, this refers to the
:ref:`prior_table@density_id` in a prior table.
Otherwise, it refers to the
:ref:`data_table@density_id` in a data table.

d_eta
*****
If the density is :ref:`density_table@Notation@Log Scaled` ,
*eta* specifies the offset in the log transformation.
Otherwise it is not used.

d_nu
****
If the density *d_id* corresponds to
``students`` or ``log_students`` ,
*nu* specifies the degrees of freedom in the Students-t distribution.
Otherwise it is not used.

index
*****
This argument has prototype

   ``size_t`` *index*

and is an identifying index for the residual.
For example, when computing the prior residuals it could be
``3`` times :ref:`var_table@var_id`
plus zero for value priors,
plus one for age difference prior, and
plus two for time difference prior.

diff
****
This argument has prototype

   ``bool`` *diff*

If *diff* is true,
this calculation is for the difference of the
random variables :math:`z` and :math:`y`.
Otherwise it is just for the random variable :math:`y`.

prior
*****
If *prior* is true,
this a prior density.
Otherwise, it is a data density.
If it is a data density, *diff* must be false.

residual
********
The return value has prototype

   ``residual_struct<`` *Float* > *residual*

residual_struct
===============
This structure has the following fields:

.. list-table::

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
If *diff* is false, *wres* is the value of

.. math::

   R(y, \mu, \delta, d)

see :ref:`weighted residual function<statistic@Weighted Residual Function, R>` .
If *diff* is true, *wres* is the value of

.. math::

   R(z, y, \mu, \delta, d)

Log Density
===========
If *diff* is false, the log-density function

.. math::

   D(y, \mu, \delta, d)

is equal to

   *logden_smooth* ``- fabs`` ( *logden_sub_abs* ))

see :ref:`log-density function<statistic@Log-Density Function, D>` .
If *diff* is true, the log-density function

.. math::

   D(z, y, \mu, \delta, d)

is equal to

   *logden_smooth* ``- fabs`` ( *logden_sub_abs* ))

Both *logden_smooth* and *logden_sub_abs*
are smooth functions of :math:`\mu` and :math:`\delta`.
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
   { }
}


namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

template <class Float>
residual_struct<Float> residual_density(
   const Float&       z          ,
   const Float&       y          ,
   const Float&       mu         ,
   const Float&       delta      ,
   density_enum       d_id       ,
   const Float&       d_eta      ,
   const Float&       d_nu       ,
   size_t             index      ,
   bool               diff       ,
   bool               prior      )
{

# ifndef NDEBUG
   if( diff )
      assert(prior);
   if( diff )
      assert( ! CppAD::isnan(z) );
# endif

   Float tiny = 10.0 / std::numeric_limits<double>::max();;
   double nan = std::numeric_limits<double>::quiet_NaN();
   double r2  = std::sqrt(2.0);
   double pi2 = 8.0 * std::atan(1.0);

   Float wres  = nan;
   Float sigma = nan;
   switch( d_id )
   {
      case uniform_enum:
      wres = 0.0;
      break;

      case gaussian_enum:
      case cen_gaussian_enum:
      case laplace_enum:
      case cen_laplace_enum:
      case students_enum:
      print_forward_if_not_positive("delta", delta);
      assert( delta > 0.0 );
      sigma = delta;
      if( diff )
         wres  = ( z - y - mu) / sigma;
      else
      {  if( CppAD::isnan(z) )
            wres = (y - mu) / sigma;
         else
            wres = (z - mu) / sigma;
      }
      break;

      case log_gaussian_enum:
      case log_laplace_enum:
      case log_students_enum:
      case cen_log_gaussian_enum:
      case cen_log_laplace_enum:
      print_forward_if_not_positive("delta", delta);
      assert( delta > 0.0 );
      assert( diff || mu + d_eta + tiny > 0.0 );
      if( diff )
      {  assert(prior);
         sigma = delta;
         wres  = ( log( z + d_eta ) - log( y + d_eta ) - mu ) / sigma;
      }
      else if( prior )
      {  print_forward_if_not_positive("mu + eta", mu + d_eta + tiny);
         sigma = log( 1.0 + delta / (mu + d_eta) );
         wres  = ( log( y + d_eta ) - log( mu + d_eta ) ) / sigma;
      }
      else // data case
      {  print_forward_if_not_positive("mu + eta", mu + d_eta + tiny);
         sigma = delta;
         if( CppAD::isnan(z) )
            wres  = ( log( y + d_eta ) - log( mu + d_eta ) ) / sigma;
         else
            wres  = ( log( z + d_eta ) - log( mu + d_eta ) ) / sigma;
      }
      break;

      default:
      assert(false);
   }
   Float logden_smooth  = nan;
   Float logden_sub_abs = nan;
   bool  censor;
   switch( d_id )
   {
      case uniform_enum:
      logden_smooth  = 0.0;
      logden_sub_abs = 0.0;
      break;

      case gaussian_enum:
      case log_gaussian_enum:
      {  logden_smooth  = - log( sigma * sqrt( pi2 ) ) - wres * wres / 2.0;
         logden_sub_abs = 0.0;
      }
      break;

      case cen_gaussian_enum:
      case cen_log_gaussian_enum:
      assert( ! diff );
      if( CppAD::isnan(z) )
         censor = y <= 0.0;
      else
         censor = z <= 0.0;
      if( censor )
      {  Float c = 0.0;
         if( d_id == cen_log_gaussian_enum )
            c = d_eta;
         Float erfc_value = CppAD::erfc( (mu - c) / ( sigma * r2 ) );
         logden_smooth    = log(erfc_value / 2.0 );
         logden_sub_abs   = 0.0;
      }
      else
      {  logden_smooth  = - log( sigma * sqrt( pi2 ) ) - wres * wres / 2.0;
         logden_sub_abs = 0.0;
      }
      break;

      case laplace_enum:
      case log_laplace_enum:
      {  logden_smooth  = - log( sigma * r2 );
         logden_sub_abs = r2 * wres;
      }
      break;

      case cen_laplace_enum:
      case cen_log_laplace_enum:
      assert( ! diff );
      if( y <= 0 )
      {  Float c = 0.0;
         if( d_id == cen_log_laplace_enum )
            c = d_eta;
         logden_smooth = - (mu - c) * r2 / sigma - std::log(2.0);
         logden_sub_abs = 0.0;
      }
      else
      {  logden_smooth  = - log( sigma * r2 );
         logden_sub_abs = r2 * wres;
      }
      break;

      case students_enum:
      case log_students_enum:
      {  Float  r       = 1.0 + wres * wres / ( d_nu - 2.0 );
         logden_smooth  =  - log( r ) * (d_nu + 1.0) / 2.0;
         logden_sub_abs = 0.0;
      }
      break;

      default:
      assert(false);
   }
   //
   residual_struct<Float> residual;
   residual.wres           = wres;
   residual.logden_smooth  = logden_smooth;
   residual.logden_sub_abs = logden_sub_abs;
   residual.density        = d_id;
   residual.index          = index;
   return residual;
}

// instantiation macro
# define DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY(Float)        \
   template residual_struct<Float> residual_density(         \
      const Float&       z            ,                     \
      const Float&       y            ,                     \
      const Float&       mu           ,                     \
      const Float&       delta        ,                     \
      density_enum       d_id         ,                     \
      const Float&       d_eta        ,                     \
      const Float&       d_nu         ,                     \
      size_t             id           ,                     \
      bool               diff         ,                     \
      bool               prior                              \
   );

// instantiations
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( double )
DISMOD_AT_INSTANTIATE_RESIDUAL_DENSITY( a1_double )

} // END DISMOD_AT_NAMESPACE
