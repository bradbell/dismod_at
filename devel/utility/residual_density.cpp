// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin residual_density$$
$spell
   mu
   enum
   const
   struct
   wres
   logden
   fabs
   bool
   var
   diff
   CppAD
$$

$section Compute Weighted Residual and Log-Density$$

$head Syntax$$
$icode%residual% = residual_density(
   %z%, %y%, %mu%, %delta%, %d_id%, %d_eta%, %d_nu%, %index%, %diff%, %prior%
)%$$

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head z$$
This argument has prototype
$codei%
   const %Float%& z
%$$
$list number$$
If $icode diff$$ is true, $icode z$$ is not nan and $icode prior$$ is true.
In this case $icode z$$ specifies the first random variable in the difference.
$lnext
If $icode diff$$ is false and $icode z$$ is not nan,
$icode prior$$ must be false and
$icode z$$ is a simulated value for the measurement $icode y$$.
$lnext
If $icode diff$$ is false and $icode z$$ is nan,
the likelihood is for the single random variable $icode y$$.
$lend

$head y$$
This argument has prototype
$codei%
   const %Float%& y
%$$
If $icode diff$$ is true,
$icode y$$ specifies the second random variable in the difference.
If $icode diff$$ is false and $icode z$$ is nan,
$icode y$$ is used both for measurement value
and for calculating log-density standard deviations.
If $icode diff$$ is false and $icode z$$ is not nan,
$icode y$$ is only used for calculating log-density standard deviations.

$head mu$$
This argument has prototype
$codei%
   const %Float%& mu
%$$
If $icode diff$$ is true, it is the central value for the difference.
Otherwise, it is the central value for $icode y$$.

$head delta$$
This argument has prototype
$codei%
   const %Float%& delta
%$$
It is either the standard deviation.
For log data densities it is in log space.
For all other cases (linear data densities or priors) it is in the
same space as $icode y$$.

$head d_id$$
This argument has prototype
$codei%
   density_enum %d_id%
%$$
If $icode prior$$ is true, this refers to the
$cref/density_id/prior_table/density_id/$$ in a prior table.
Otherwise, it refers to the
$cref/density_id/data_table/density_id/$$ in a data table.

$head d_eta$$
If the density is $cref/log scaled/density_table/Notation/Log Scaled/$$,
$icode eta$$ specifies the offset in the log transformation.
Otherwise it is not used.

$head d_nu$$
If the density $icode d_id$$ corresponds to
$code students$$ or $code log_students$$,
$icode nu$$ specifies the degrees of freedom in the Students-t distribution.
Otherwise it is not used.

$head index$$
This argument has prototype
$codei%
   size_t %index%
%$$
and is an identifying index for the residual.
For example, when computing the prior residuals it could be
$code 3$$ times $cref/var_id/var_table/var_id/$$
plus zero for value priors,
plus one for age difference prior, and
plus two for time difference prior.

$head diff$$
This argument has prototype
$codei%
   bool %diff%
%$$
If $icode diff$$ is true,
this calculation is for the difference of the
random variables $latex z$$ and $latex y$$.
Otherwise it is just for the random variable $latex y$$.

$head prior$$
If $icode prior$$ is true,
this a prior density.
Otherwise, it is a data density.
If it is a data density, $icode diff$$ must be false.

$head residual$$
The return value has prototype
$codei%
   residual_struct<%Float%> %residual%
%$$

$subhead residual_struct$$
This structure has the following fields:
$table
Type $cnext Field $cnext Description $rnext
$icode Float$$ $cnext
   $code wres$$ $cnext
   $cref/weighted residual/statistic/Weighted Residual Function, R/$$
$rnext
$icode Float$$ $cnext
   $code logden_smooth$$ $cnext
   this smooth term is in
   $cref/log-density/statistic/Log-Density Function, D/$$
$rnext
$icode Float$$ $cnext
   $code logden_sub_abs$$ $cnext
   absolute value of this smooth term is in log-density
$rnext
$code density_enum$$ $cnext
   $code density$$ $cnext
   type of density function; see
   $cref/density_enum/get_density_table/density_enum/$$
$rnext
$code size_t$$ $cnext
   $code index$$ $cnext
   identifier for this residual; see
   $cref/index/residual_density/index/$$ above.
$tend

$subhead wres$$
If $icode diff$$ is false, $icode wres$$ is the value of
$latex \[
   R(y, \mu, \delta, d)
\]$$
see $cref/weighted residual function
   /statistic
   /Weighted Residual Function, R
/$$.
If $icode diff$$ is true, $icode wres$$ is the value of
$latex \[
   R(z, y, \mu, \delta, d)
\]$$

$subhead Log Density$$
If $icode diff$$ is false, the log-density function
$latex \[
   D(y, \mu, \delta, d)
\]$$
is equal to
$codei%
   %logden_smooth% - fabs(%logden_sub_abs)%)
%$$
see $cref/log-density function
   /statistic
   /Log-Density Function, D
/$$.
If $icode diff$$ is true, the log-density function
$latex \[
   D(z, y, \mu, \delta, d)
\]$$
is equal to
$codei%
   %logden_smooth% - fabs(%logden_sub_abs)%)
%$$
Both $icode logden_smooth$$ and $icode logden_sub_abs$$
are smooth functions of $latex \mu$$ and $latex \delta$$.
This expresses the log-density
in terms of smooth functions (for optimization purposes).

$subhead Absolute Value Terms$$
If $icode logden_sub_abs$$ is a CppAD constant,
(not a dynamic parameter or variable),
there is no absolute value term for this residual.


$end
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
