// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin data_model_ctor$$
$spell
   var
   bool
   avgint
   logden
   std
   vec
   const
   enum
   struct
   Cpp
   obj
   covariate
   covariates
   cov
   cv
   eigen
   sim_stdcv
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Data Model: Constructor$$

$head Syntax$$
$codei%data_model %data_object%(
   %fit_simulated_data%,
   %meas_noise_effect%,
   %rate_case%,
   %bound_random%,
   %n_covariate%,
   %ode_step_size%,
   %age_avg_grid%,
   %age_table%,
   %time_table%,
   %subgroup_table%,
   %integrand_table%,
   %prior_table%,
   %subset_object%,
   %subset_cov_value%,
   %w_info_vec%,
   %s_info_vec%,
   %pack_object%,
   %child_info4data%
)%$$

$head Prototype$$
$srcthisfile%
   0%// BEGIN_DATA_MODEL_PROTOTYPE%// END_DATA_MODEL_PROTOTYPE%1
%$$

$head data_object$$
This is the $code data_model$$ object being constructed.

$head fit_simulated_data$$
If this is true, we are fitting simulated data that comes from the
$cref data_sim_table$$.

$head meas_noise_effect$$
This is the value of
$cref/meas_noise_effect/option_table/meas_noise_effect/$$ in the option table.

$head rate_case$$
This is the value of
$cref/rate_case/option_table/rate_case/$$ in the option table.

$head bound_random$$
This is the
$cref/bound_random/option_table/Optimize Random Only/bound_random/$$.

$head n_covariate$$
This is the number of covariates; i.e., number or rows in
$cref covariate_table$$.

$head ode_step_size$$
This is the $cref/
   ode_step_size/
   option_table/
   Age Average Grid/
   ode_step_size
/$$.

$head age_avg_grid$$
This is the $cref/age average grid/age_avg_table/Age Average Grid/$$.

$head age_table$$
This is the $cref/age_table/get_age_table/age_table/$$.

$head time_table$$
This is the $cref/time_table/get_time_table/time_table/$$.

$head subgroup_table$$
This is the $cref/subgroup_table/get_subgroup_table/subgroup_table/$$.

$head integrand_table$$
This is the $cref/integrand_table/get_integrand_table/integrand_table/$$.

$head subset_object$$
This is the sub-sampled version of the data or avgint table; see
$cref/subset_data_obj/subset_data/subset_data_obj/$$,
$cref/avgint_subset_obj/avgint_subset/avgint_subset_obj/$$.

$head subset_cov_value$$
This is the sub-sampled version of the covariates; see
$cref/subset_data_cov_value/subset_data/subset_data_cov_value/$$,
$cref/avgint_subset_cov_value/avgint_subset/avgint_subset_cov_value/$$.
A reference to $icode subset_cov_value$$ is used by $icode data_object$$
(so $icode age_table$$ cannot be deleted for as long as
$icode data_object$$ is used).

$head w_info_vec$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
   %w_info_vec%[ %weight_id% ]
%$$
is the corresponding $cref weight_info$$ information.
In addition, the constant weight is included at the end of the vector; i.e.,
at index $icode%w_info_vec%.size()-1%$$.

$head s_info_vec$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
   %s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.
For each $icode%s_info_vec%[%smooth_id%]%$$ object,
only the following functions are used:
$code age_size$$, $code time_size$$, $code age_id$$, $code time_id$$.

$head pack_object$$
This is the $cref pack_info$$ information corresponding to
the $cref model_variables$$.
A reference to $icode pack_object$$ is used by $icode data_object$$.

$head child_info4data$$
This is the $cref child_info$$ information corresponding to
parent node, node table, and data table.

$head data_object.n_covariate_$$
set to $icode n_covariate$$.

$head data_object.ode_step_size_$$
set to $icode ode_step_size$$.

$head data_object.pack_object_$$
set to $icode pack_object$$.

$head data_object.minimum_meas_cv_$$
Set to a vector with the same size as $icode integrand_table$$ and
$codei%
   %minimum_meas_cv_[%integrand_id%] =
      %integrand_table%[%integrand_id%].minimum_meas_cv
%$$

$head replace_like_called_$$
initialize as false.

$head n_child_$$
set to number of children.

$head subset_data_obj_$$
for each $icode subset_id$$, set $codei%subset_data_obj_[%subset_id%]%$$
fields that are command both data_subset and avgint_subset.

$head data_info_$$
for each $icode subset_id$$, set
$codei%data_info_[%subset_id%]%$$
is extra information for each data point.
Each of the fields in
$codei%data_info_[%subset_id%]%$$
is described below:

$subhead density$$
Is the
$cref/density_enum/get_density_table/density_enum/$$ corresponding
to the $icode subset_id$$.

$subhead child$$
This $code size_t$$ value is the
$cref/child/child_info/table_id2child/child/$$ index corresponding
to this $icode subset_id$$.
Note that if it is equal to $code n_child_$$,
this data point corresponds to th parent node.

$subhead depend_on_ran_var$$
This $code bool$$ value is true (false) if the data point corresponding to
$icode subset_id$$ depends (does not depend) on a random effect
that is a variable; i.e., not constrained to be a constant.


$head avgint_obj_$$
The $code avg_integrand$$ $cref/constructor/avg_integrand_ctor/$$
is used to create this argument.
The arguments to this constructor are a subset of the
arguments to the $code data_model$$ constructor and are passed through
(with the same name) to the $code avg_integrand$$ constructor.

$end
-----------------------------------------------------------------------------
*/
# include <cppad/mixed/exception.hpp>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/data_model.hpp>
# include <dismod_at/child_info.hpp>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/residual_density.hpp>
# include <dismod_at/a1_double.hpp>
# include <dismod_at/avgint_subset.hpp>
# include <dismod_at/null_int.hpp>
# include <dismod_at/error_exit.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// destructor
data_model::~data_model(void)
{ }

// BEGIN_DATA_MODEL_PROTOTYPE
template <class SubsetStruct>
data_model::data_model(
   bool                                     fit_simulated_data ,
   const std::string&                       meas_noise_effect  ,
   const std::string&                       rate_case          ,
   double                                   bound_random       ,
   size_t                                   n_covariate        ,
   double                                   ode_step_size      ,
   const CppAD::vector<double>&             age_avg_grid       ,
   const CppAD::vector<double>&             age_table          ,
   const CppAD::vector<double>&             time_table         ,
   const CppAD::vector<subgroup_struct>&    subgroup_table     ,
   const CppAD::vector<integrand_struct>&   integrand_table    ,
   const CppAD::vector<mulcov_struct>&      mulcov_table       ,
   const CppAD::vector<prior_struct>&       prior_table        ,
   const CppAD::vector<SubsetStruct>&       subset_object      ,
   const CppAD::vector<double>&             subset_cov_value   ,
   const CppAD::vector<weight_info>&        w_info_vec         ,
   const CppAD::vector<smooth_info>&        s_info_vec         ,
   const pack_info&                         pack_object        ,
   const child_info&                        child_info4data    )
// END_DATA_MODEL_PROTOTYPE
:
// const
fit_simulated_data_ ( fit_simulated_data)           ,
n_covariate_        (n_covariate)                   ,
ode_step_size_      (ode_step_size)                 ,
n_child_            ( child_info4data.child_size() )   ,
subset_cov_value_   (subset_cov_value)              ,
pack_object_        (pack_object)                   ,
avgint_obj_(
   ode_step_size,
   rate_case,
   age_avg_grid,
   age_table,
   time_table,
   subgroup_table,
   integrand_table,
   mulcov_table,
   w_info_vec,
   s_info_vec,
   pack_object
),
avg_noise_obj_(
   ode_step_size,
   age_avg_grid,
   age_table,
   time_table,
   subgroup_table,
   integrand_table,
   w_info_vec,
   s_info_vec,
   pack_object
)
{  assert( bound_random >= 0.0 );
   assert( n_child_ == pack_object.child_size() );
   // ------------------------------------------------------------------------
   //
   using std::string;
   //
   // meas_noise_effect_
   // add_std
   if( meas_noise_effect == "add_std_scale_all" )
      meas_noise_effect_ = add_std_scale_all_enum;
   else if( meas_noise_effect == "add_std_scale_none" )
      meas_noise_effect_ = add_std_scale_none_enum;
   else if( meas_noise_effect == "add_std_scale_log" )
      meas_noise_effect_ = add_std_scale_log_enum;
   // add_var
   else if( meas_noise_effect == "add_var_scale_all" )
      meas_noise_effect_ = add_var_scale_all_enum;
   else if( meas_noise_effect == "add_var_scale_none" )
      meas_noise_effect_ = add_var_scale_none_enum;
   else if( meas_noise_effect == "add_var_scale_log" )
      meas_noise_effect_ = add_var_scale_log_enum;
   else
      assert(false);
   //
   // minimum_meas_cv_
   minimum_meas_cv_.resize( integrand_table.size() );
   for(size_t i = 0; i < integrand_table.size(); ++i)
      minimum_meas_cv_[i] = integrand_table[i].minimum_meas_cv;
   //
   // replace_like_called_: initialize
   replace_like_called_ = false;
   //
   // -----------------------------------------------------------------------
   // subset_data_obj_
   //
   // only set values that are in subset_data_struct and avgint_subset_struct
   size_t n_subset = subset_object.size();
   subset_data_obj_.resize(n_subset);
   assert( subset_cov_value.size() == n_covariate * n_subset );
   for(size_t i = 0; i < n_subset; i++)
   {  subset_data_obj_[i].original_id  = subset_object[i].original_id;
      subset_data_obj_[i].integrand_id = subset_object[i].integrand_id;
      subset_data_obj_[i].node_id      = subset_object[i].node_id;
      subset_data_obj_[i].subgroup_id  = subset_object[i].subgroup_id;
      subset_data_obj_[i].weight_id    = subset_object[i].weight_id;
      subset_data_obj_[i].age_lower    = subset_object[i].age_lower;
      subset_data_obj_[i].age_upper    = subset_object[i].age_upper;
      subset_data_obj_[i].time_lower   = subset_object[i].time_lower;
      subset_data_obj_[i].time_upper   = subset_object[i].time_upper;
   }
   // -----------------------------------------------------------------------
   // data_info_
   //
   // has same size as subset_data_obj
   data_info_.resize( n_subset );
   //
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  // information for this data point
      size_t original_id = subset_object[subset_id].original_id;

      // integrand as an enum instead of integrand_id
      size_t  integrand_id     = subset_object[subset_id].integrand_id;
      integrand_enum integrand = integrand_table[integrand_id].integrand;

      // child of parent node that this data is associated with
      size_t  child            = child_info4data.table_id2child(original_id);

      // group_id for this data point
      size_t subgroup_id       = subset_object[subset_id].subgroup_id;
      size_t group_id          = subgroup_table[subgroup_id].group_id;

      // set child information for this data point
      data_info_[subset_id].child     = child;

      // Does this data point depend on the random effects
      // that do not have equal bounds
      bool depend_on_ran_var = false;
      if( bound_random > 0.0 )
      {  CppAD::vector<size_t> rate_id_vec;
         switch( integrand )
         {  case Sincidence_enum:
            rate_id_vec.push_back( size_t(iota_enum) );
            break;

            case remission_enum:
            rate_id_vec.push_back( size_t(rho_enum) );
            break;

            case mtexcess_enum:
            rate_id_vec.push_back( size_t(chi_enum) );
            break;

            case mtother_enum:
            rate_id_vec.push_back( size_t(omega_enum) );
            break;

            case mtwith_enum:
            case relrisk_enum:
            rate_id_vec.push_back( size_t(chi_enum) );
            rate_id_vec.push_back( size_t(omega_enum) );
            break;

            default:
            for(size_t ell = 0; ell < number_rate_enum; ell++)
               rate_id_vec.push_back( ell );
            break;
         }
         // change depend_on_ran_var for rate effects that are not constant
         if( child < n_child_ )
         for(size_t ell = 0; ell < rate_id_vec.size(); ell++)
         {  size_t rate_id = rate_id_vec[ell];
            // check if any random effects for this rate are not constant
            size_t smooth_id =
               pack_object.node_rate_value_info(rate_id, child).smooth_id;
            if( smooth_id != DISMOD_AT_NULL_SIZE_T )
            {  const smooth_info& s_info = s_info_vec[smooth_id];
               depend_on_ran_var |= ! s_info.all_const_value();
            }
         }
         // change depend_on_ran_var for subgroup_rate_value covariates
         for(size_t ell = 0; ell < rate_id_vec.size(); ell++)
         {  size_t rate_id = rate_id_vec[ell];
            size_t n_cov = pack_object.subgroup_rate_value_n_cov(rate_id);
            for(size_t j = 0; j < n_cov; ++j)
            {  const pack_info::subvec_info info_0 =
                  pack_object.subgroup_rate_value_info(rate_id, j, 0);
               size_t n_sub =
                  pack_object.subgroup_rate_value_n_sub(rate_id, j);
               if( info_0.group_id == group_id )
               for(size_t k = 0; k < n_sub; ++k)
               {  const pack_info::subvec_info info =
                  pack_object.subgroup_rate_value_info(rate_id, j, k);
                  //
                  assert( info.group_id  == info_0.group_id );
                  assert( info.smooth_id == info_0.smooth_id );
                  //
                  size_t smooth_id = info.smooth_id;
                  assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
                  const smooth_info& s_info = s_info_vec[smooth_id];
                  depend_on_ran_var |= ! s_info.all_const_value();
               }
            }
         }
         // change depend_on_ran_var for subgroup_meas_value covariates
         size_t n_cov = pack_object.subgroup_meas_value_n_cov(integrand_id);
         for(size_t j = 0; j < n_cov; ++j)
         {  const pack_info::subvec_info info_0 =
               pack_object.subgroup_meas_value_info(integrand_id, j, 0);
            size_t n_sub =
               pack_object.subgroup_meas_value_n_sub(integrand_id, j);
            if( info_0.group_id == group_id )
            for(size_t k = 0; k < n_sub; ++k)
            {  const pack_info::subvec_info info =
               pack_object.subgroup_meas_value_info(integrand_id, j, k);
               //
               assert( info.group_id  == info_0.group_id );
               assert( info.smooth_id == info_0.smooth_id );
               //
               size_t smooth_id = info.smooth_id;
               assert( smooth_id != DISMOD_AT_NULL_SIZE_T );
               const smooth_info& s_info = s_info_vec[smooth_id];
               depend_on_ran_var |= ! s_info.all_const_value();
            }
         }
      }
      data_info_[subset_id].depend_on_ran_var = depend_on_ran_var;
   }
}
/*
$begin data_model_replace_like$$
$spell
   avgint
   const
   CppAD
   struct
   std
   obj
   cv
   sim_stdcv
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Set Value Necessary for Likelihood (not for Average Integrand)$$

$head Syntax$$
$icode%data_object%.replace_like(%subset_data_obj%)%$$

$head Purpose$$
The values
$cref/density_id/data_table/density_id/$$,
$cref/hold_out/data_table/hold_out/$$,
$cref/meas_value/data_table/meas_value/$$,
$cref/meas_std/data_table/meas_std/$$,
$cref/eta/data_table/eta/$$,
$cref/nu/data_table/nu/$$,
are not necessary to calculate the average integrand.
However, the are necessary to use the functions
$cref/data_object.like_one/data_model_like_one/$$ and
$cref/data_object.like_all/data_model_like_all/$$.
If the $cref/data_sim_value/data_sim_table/data_sim_value/$$
values in $icode subset_data_obj$$ are not nan, these
are simulated values used in calculating the likelihood.

$head data_object$$
This object has prototype
$codei%
   data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.

$subhead subset_object$$
We use $icode subset_object$$
for the $cref/subset_object/data_model_ctor/$$ in the $icode data_object$$
constructor.

$subhead n_subset$$
We use the notation
$codei%
   %n_subset% = %subset_object%.size()
%$$

$subhead subset_id$$
This an index between zero and $icode%n_subset% - 1%$$.
It is used to refer to the corresponding element of
$icode subset_object$$.

$head subset_data_obj$$
This argument has prototype
$codei%
   const CppAD::vector<subset_data_struct>& %subset_data_obj%
%$$
and has size $icode n_subset$$.
For each $icode subset_id$$,
and for $icode%field% = density_id%,% hold_out%,% meas_value%,% meas_std%$$,
$icode%subset_data_obj%[%subset_id%].%field%$$,
is used as a replacement for
$icode%subset_object[%subset_id%]%.%field%$$.

$head Laplace Density$$
The Laplace density is only allowed if the model for the
$cref avg_integrand$$ does not depend on the value of the random effects.
If this assumption is violated, an error messages is generated and the
program exits.

$end
*/
void data_model::replace_like(
      const CppAD::vector<subset_data_struct>&  subset_data_obj )
{
   // n_subset
   size_t n_subset = subset_data_obj_.size();
   assert( subset_data_obj.size() == n_subset );
   //
   // replace density_id, hold_out, meas_value, meas_std, eta, nu
   for(size_t subset_id = 0; subset_id < n_subset; subset_id++)
   {  subset_data_obj_[subset_id].density =
         subset_data_obj[subset_id].density;
      subset_data_obj_[subset_id].hold_out =
         subset_data_obj[subset_id].hold_out;
      subset_data_obj_[subset_id].meas_value =
         subset_data_obj[subset_id].meas_value;
      subset_data_obj_[subset_id].meas_std =
         subset_data_obj[subset_id].meas_std;
      subset_data_obj_[subset_id].eta =
         subset_data_obj[subset_id].eta;
      subset_data_obj_[subset_id].nu =
         subset_data_obj[subset_id].nu;
      subset_data_obj_[subset_id].data_sim_value =
         subset_data_obj[subset_id].data_sim_value;
      //
      data_info_[subset_id].density = subset_data_obj[subset_id].density;
      //
      bool laplace = data_info_[subset_id].density == laplace_enum;
      laplace     |= data_info_[subset_id].density == log_laplace_enum;
      if( laplace && data_info_[subset_id].depend_on_ran_var )
      {  std::string msg, table_name;
         size_t data_id = subset_data_obj_[subset_id].original_id;
         table_name = "data";
         msg  = "density_id corresponds to laplace or log_laplace and\n";
         msg += "model depends on random effects that are not constrained";
         error_exit(msg, table_name, data_id);
      }
   }
   replace_like_called_ = true;
   return;
}

/*
-----------------------------------------------------------------------------
$begin data_model_average$$

$spell
   vec
   const
   CppAD
$$
$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section Data Model: Compute One Average Integrand$$

$head Syntax$$
$icode%avg% = %data_object%.average(%subset_id%, %pack_vec%)%$$

$head data_object$$
This object has prototype
$codei%
   data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.
The object $icode data_object$$ is effectively const.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head subset_id$$
This argument has prototype
$codei%
   size_t %subset_id%
%$$
and is the $cref/subset_id/subset_data/subset_data_obj/subset_id/$$
we are computing the average integrand for.

$head pack_vec$$
This argument has prototype
$codei%
   const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$.

$head avg$$
The return value has prototype
$codei%
   %Float% avg
%$$
This is the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$
for the specified data point.

$children%example/devel/model/avg_no_ode_xam.cpp
   %example/devel/model/avg_yes_ode_xam.cpp
%$$
$head Example$$
The files
$cref avg_no_ode_xam.cpp$$ and $cref avg_yes_ode_xam.cpp$$
contain examples using this routine.

$end
*/
template <class Float>
Float data_model::average(
   size_t                        subset_id ,
   const CppAD::vector<Float>&   pack_vec  )
{
   // arguments to avg_integrand::rectangle
   const subset_data_struct& data_item = subset_data_obj_[subset_id];
   double age_lower    = data_item.age_lower;
   double age_upper    = data_item.age_upper;
   double time_lower   = data_item.time_lower;
   double time_upper   = data_item.time_upper;
   size_t weight_id    = size_t( data_item.weight_id );
   size_t integrand_id = size_t( data_item.integrand_id );
   size_t subgroup_id  = size_t( data_item.subgroup_id );
   size_t child        = size_t( data_info_[subset_id].child );
   CppAD::vector<double> x(n_covariate_);
   for(size_t j = 0; j < n_covariate_; j++)
      x[j] = subset_cov_value_[subset_id * n_covariate_ + j];
   //
   // compute average integrand
   Float result = avgint_obj_.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      integrand_id,
      n_child_,
      child,
      subgroup_id,
      x,
      pack_vec
   );
   //
   assert( ! CppAD::isnan(result) );
   return result;
}

/*
-----------------------------------------------------------------------------
$begin data_model_like_one$$

$spell
   sim_stdcv
   avgint
   Sincidence
   Tincidence
   struct
   logden
   fabs
   logden
   std
   Integrands
   wres
   var
   vec
   const
   CppAD
   subvectors
   mtspecific
   mtall
   mtstandard
   mtexcess
   mtwith
   mtother
   relrisk
   xam
$$
$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section One Weighted Residual and Log-Likelihood for any Integrands$$

$head Syntax$$
$icode%residual% = %data_object%.like_one(
   %subset_id%, %pack_vec%, %avg%, %delta_out%
)%$$

$head Requirement$$
One must call $cref/replace_like/data_model_replace_like/$$
before calling this function.

$head Log-likelihood$$
We use $cref/y_i/data_like/Notation/y_i/$$ to denote the
$cref/meas_value/data_table/meas_value/$$ corresponding
to this $cref/subset_id/subset_data/subset_data_obj/subset_id/$$.
The log-likelihood computed by $code like_one$$ is the mapping
$latex \[
   \ell (u, \theta) = C + \log [ \B{p} ( y_i | u , \theta ) ]
\] $$
where $latex u$$ are the random effects,
$latex \theta$$ are the fixed effects, and
$latex C$$ is a constant that does
not depend on $latex ( u , \theta )$$.

$head data_object$$
This object has prototype
$codei%
   data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.
It is effectively const
(some internal arrays are used for temporary work space and kept between
calls to avoid re-allocating memory).

$subhead fit_simulated_data$$
If $cref/fit_simulated_data/data_model_ctor/fit_simulated_data/$$ is true,
we are fitting simulated data $icode data_sim_value$$,
otherwise we are fitting the actual data $icode meas_value$$.

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head subset_id$$
This argument has prototype
$codei%
   size_t %subset_id%
%$$
and is the $cref/subset_id/subset_data/subset_data_obj/subset_id/$$
we are computing the weighted residual and log-likelihood for.

$head pack_vec$$
This argument has prototype
$codei%
   const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$; i.e.,
$latex (u , \theta)$$.
Only the $cref pack_info_group_meas$$ subvectors of $icode pack_vec$$ are used
by $code like_one$$ (note that other components of $latex (u, \theta )$$
are used to compute $icode avg$$ documented below).

$head avg$$
This argument has prototype
$codei%
   const %Float%& %avg%
%$$
and is the
$cref/average integrand/avg_integrand/Average Integrand, A_i/$$,
$latex A_i ( u , \theta )$$, for the specified data point.
This can be calculated using $cref data_model_average$$.

$head delta_out$$
The input value of $icode delta_out$$ does not matter.
Upon return it is the adjusted standard deviation
$cref/delta/data_like/Adjusted Standard Deviation, delta_i(theta)/$$
for this data point.

$head residual$$
The return value has prototype
$codei%
   residual_struct<%Float%> %residual%
%$$
see $cref/residual_struct/residual_density/residual/residual_struct/$$.
It contains the weighted residual and the corresponding log-density.

$subhead index$$
Note that the $cref/index/residual_density/index/$$ field is set to
the value $icode subset_id$$.

$children%example/devel/model/like_one_xam.cpp
%$$
$head Example$$
The file $cref like_one_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
template <class Float>
residual_struct<Float> data_model::like_one(
   size_t                        subset_id ,
   const CppAD::vector<Float>&   pack_vec  ,
   const Float&                  avg       ,
   Float&                        delta_out )
{
   assert( pack_object_.size() == pack_vec.size() );
   assert( replace_like_called_ );

   // covariate information for this data point
   CppAD::vector<double> x(n_covariate_);
   for(size_t j = 0; j < n_covariate_; j++)
      x[j] = subset_cov_value_[subset_id * n_covariate_ + j];
   double eta          = subset_data_obj_[subset_id].eta;
   double nu           = subset_data_obj_[subset_id].nu;
   double meas_value   = subset_data_obj_[subset_id].meas_value;
   double meas_std     = subset_data_obj_[subset_id].meas_std;
   double age_lower    = subset_data_obj_[subset_id].age_lower;
   double age_upper    = subset_data_obj_[subset_id].age_upper;
   double time_lower   = subset_data_obj_[subset_id].time_lower;
   double time_upper   = subset_data_obj_[subset_id].time_upper;
   size_t weight_id    = size_t( subset_data_obj_[subset_id].weight_id );
   size_t subgroup_id  = size_t( subset_data_obj_[subset_id].subgroup_id );
   size_t integrand_id = size_t( subset_data_obj_[subset_id].integrand_id );
   //
   double data_sim_value   = subset_data_obj_[subset_id].data_sim_value;
   //
   // average noise effect
   Float std_effect = avg_noise_obj_.rectangle(
      age_lower,
      age_upper,
      time_lower,
      time_upper,
      weight_id,
      subgroup_id,
      integrand_id,
      x,
      pack_vec
   );
   //
   assert( meas_std  > 0.0 );
   double meas_cv = minimum_meas_cv_[integrand_id];
   double Delta   = std::max(meas_std, meas_cv * std::fabs(meas_value) );
   density_enum density = data_info_[subset_id].density;
   //
   // transformed standard deviation
   double sigma = Delta;
   if( log_density(density) )
      sigma = log( meas_value + eta + Delta ) - log( meas_value + eta );
   //
   // Compute the adusted standard deviation, delta_out
   switch( meas_noise_effect_ )
   {  // add_std
      case add_std_scale_all_enum:
      delta_out  = sigma * (1.0  + std_effect);
      break;
      case add_std_scale_none_enum:
      delta_out  = sigma + std_effect;
      break;
      case add_std_scale_log_enum:
      if( log_density(density) )
         delta_out  = sigma * (1.0  + std_effect);
      else
         delta_out  = sigma + std_effect;
      break;
      // -----------------------------------------------------------------
      // add_var
      case add_var_scale_all_enum:
      delta_out  = sigma * sqrt(1.0  + std_effect);
      break;
      case add_var_scale_none_enum:
      delta_out  = sqrt( sigma * sigma + std_effect );
      break;
      case add_var_scale_log_enum:
      if( log_density(density) )
         delta_out  = sigma * sqrt(1.0  + std_effect);
      else
         delta_out  = sqrt( sigma * sigma + std_effect );
      break;
      // -----------------------------------------------------------------
      default:
      assert(false);
   }
   //
   bool diff  = false;
   bool prior = false;
# ifndef NDEBUG
   if( fit_simulated_data_ )
      assert( ! std::isnan(data_sim_value) );
   else
      assert( std::isnan(data_sim_value) );
# endif
   struct residual_struct<Float> residual = residual_density(
      Float(data_sim_value),
      Float(meas_value),
      avg,
      delta_out,
      density,
      Float(eta),
      Float(nu),
      subset_id,
      diff,
      prior
   );
   //
   return residual;
}
/*
-------------------------------------------------------------------------------
$begin data_model_like_all$$
$spell
   avgint
   bool
   enum
   vec
   CppAD
   const
   struct
$$

$nospell
$bold This is dismod_at-20221105 documentation:$$ Here is a link to its
$href%https://dismod-at.readthedocs.io%current documentation%$$.
$$
$section All the Weighted Residuals and Log-Likelihoods$$

$head Syntax$$
$icode%residual_vec% = %data_object%.like_all(
   %hold_out%, %random_depend%, %pack_vec%
)%$$

$head Requirement$$
One must call $cref/replace_like/data_model_replace_like/$$
before calling this function.

$head data_object$$
This object has prototype
$codei%
   data_model %data_object%
%$$
see $cref/data_object constructor/data_model_ctor/data_object/$$.
It is effectively const
(some internal arrays are used for temporary work space and kept between
calls to avoid re-allocating memory).

$head Float$$
The type $icode Float$$ must be $code double$$ or
$cref a1_double$$.

$head hold_out$$
This argument has prototype
$codei%
   bool %hold_out%
%$$
If it is true, the flagged data will be held out,
otherwise it will not.

$head random_depend$$
This argument has prototype $codei%
   bool %random_depend%
%$$
If it is true (false), only residuals that depend on the random effects
that have non-equal (equal) bounds, are included.

$head pack_vec$$
This argument has prototype
$codei%
   const CppAD::vector<%Float%>& %pack_vec%
%$$
and is all the $cref model_variables$$ in the order
specified by $cref pack_info$$; i.e., $latex (u , \theta)$$.
If $icode random_depend$$ is false,
only the components of $icode u$$ that have equal lower and upper
bounds are defined (the other components are unspecified).

$head residual_vec$$
The return value has prototype
$codei%
   CppAD::vector< residual_struct<%Float%> > %residual_vec%
%$$

$subhead index$$
For each element of $icode residual_vec$$,
the residual $cref/index/residual_density/index/$$
is set to the $cref/data_subset_id/data_subset_table/data_subset_id/$$
for the corresponding residual.

$subhead Include Hold Outs$$
If $icode hold_out$$ is false,
the size of $icode residual$$ is equal the number of subset data values
$cref/n_subset/subset_data/subset_data_obj/n_subset/$$.
The order of the residuals is the same as in
$cref/subset_object/data_model_ctor/subset_object/$$ in the
$icode data_object$$ constructor.

$subhead No Hold Outs$$
If $icode hold_out$$ is true,
the size of $icode residual$$ is equal the number of subset data values
$cref/n_subset/subset_data/subset_data_obj/n_subset/$$,
minus the number for which
$cref/hold_out/data_table/hold_out/$$ is one.
The order of the residuals is not specified in this case.

$head Log Density$$
The log of the density
$latex \B{p}( y | u , \theta )$$,
for the data that is included,
is the sum of the log of the densities corresponding to all the
$cref/residuals/residual_density/$$ in $icode residual_vec$$.

$end
*/
template <class Float>
CppAD::vector< residual_struct<Float> > data_model::like_all(
   bool                        hold_out      ,
   bool                        random_depend ,
   const CppAD::vector<Float>& pack_vec      )
{  assert( replace_like_called_ );
   //
   // loop over the subsampled data
   CppAD::vector< residual_struct<Float> > residual_vec;
   for(size_t subset_id = 0; subset_id < subset_data_obj_.size(); subset_id++)
   {  bool keep = hold_out == false;
      keep     |= subset_data_obj_[subset_id].hold_out == 0;
      if( random_depend )
         keep &= data_info_[subset_id].depend_on_ran_var == true;
      else
         keep &= data_info_[subset_id].depend_on_ran_var == false;
      assert( data_info_[subset_id].child <= n_child_ );
      if( keep )
      {  Float avg = average(subset_id, pack_vec);

         // compute its residual and log likelihood
         Float not_used;
         residual_struct<Float> residual =
            like_one(subset_id, pack_vec, avg, not_used);
         residual_vec.push_back( residual );
      }
   }
   return residual_vec;
}

// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(SubsetStruct)       \
template data_model::data_model(                                   \
   bool                                     fit_simulated_data ,  \
   const std::string&                       meas_noise_effect  ,  \
   const std::string&                       rate_case          ,  \
   double                                   bound_random       ,  \
   size_t                                   n_covariate        ,  \
   double                                   ode_step_size      ,  \
   const CppAD::vector<double>&             age_avg_grid       ,  \
   const CppAD::vector<double>&             age_table          ,  \
   const CppAD::vector<double>&             time_table         ,  \
   const CppAD::vector<subgroup_struct>&    subgroup_table     ,  \
   const CppAD::vector<integrand_struct>&   integrand_table    ,  \
   const CppAD::vector<mulcov_struct>&      mulcov_table       ,  \
   const CppAD::vector<prior_struct>&       prior_table        ,  \
   const CppAD::vector<SubsetStruct>&       subset_object      ,  \
   const CppAD::vector<double>&             subset_cov_value   ,  \
   const CppAD::vector<weight_info>&        w_info_vec         ,  \
   const CppAD::vector<smooth_info>&        s_info_vec         ,  \
   const pack_info&                         pack_object        ,  \
   const child_info&                        child_info4data       \
);
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(subset_data_struct)
DISMOD_AT_INSTANTIATE_DATA_MODEL_CTOR(avgint_subset_struct)
// ------------------------------------------------------------------------
# define DISMOD_AT_INSTANTIATE_DATA_MODEL(Float)            \
   template Float data_model::average(                     \
      size_t                        subset_id,            \
      const CppAD::vector<Float>&   pack_vec              \
   );                                                      \
   template residual_struct<Float>                         \
   data_model::like_one(                                   \
      size_t                        subset_id,            \
      const CppAD::vector<Float>&   pack_vec ,            \
      const Float&                  avg      ,            \
      Float&                        delta_out             \
   );                                                      \
   template CppAD::vector< residual_struct<Float> >        \
   data_model::like_all(                                   \
      bool                          hold_out ,            \
      bool                          parent   ,            \
      const CppAD::vector<Float>&   pack_vec              \
   );                                                      \

// instantiations
DISMOD_AT_INSTANTIATE_DATA_MODEL( double )
DISMOD_AT_INSTANTIATE_DATA_MODEL( a1_double )


} // END DISMOD_AT_NAMESPACE
