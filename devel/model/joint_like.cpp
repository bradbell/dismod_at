// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin devel_joint_like_ctor$$
$spell
	vec
	var
	const
	CppAD
	struct
	std
$$

$section Joint Likelihood$$

$head Under Construction$$

$head Syntax$$
$codei%joint_like %joint%(
	// arguments in same order as for data_model constructor
	%parent_node_id%,
	%n_age_ode%,
	%n_time_ode%,
	%ode_step_size%,
	%age_table%,
	%time_table%,
	%integrand_table%,
	%node_table%,
	%data_table%,
	%w_info_vec%,
	%s_info_vec%,
	// arguments to prior_density that are not in data_model constructor
	%var_info%,
	%prior_table%,
)%$$

$head Purpose$$
This object can be used to evaluate the joint likelihood of the
$cref/fixed effects/model_variable/Fixed Effects, theta/$$, $latex \theta$$,
$cref/random effects/model_variable/Random Effects, u/$$, $latex u$$,
and the
$cref/measurement vector/data_like/Data Table Notation/y_i/$$, $latex y$$
as a function of the fixed and random effects; i.e., to evaluate
$latex \[
	\B{p} ( y | u , \theta ) \B{p} ( u | \theta ) \B{p} ( \theta )
\] $$
up to a constant multiple that does not depend on
$latex u$$ or $latex \theta$$.

$head parent_node_id$$
This argument has prototype
$codei%
	size_t %parent_node_id%
%$$
and is the run table
$cref/parent_node_id/run_table/parent_node_id/$$.

$head n_age_ode$$
This argument has prototype
$codei%
	size_t %n_age_ode%
%$$
It is the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$head n_time_ode$$
This argument has prototype
$codei%
	size_t %n_time_ode%
%$$
It is the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$head ode_step_size$$
This argument has prototype
$codei%
	double %ode_step_size%
%$$
and is the value of $cref/ode_step_size/run_table/ode_step_size/$$
in the run table.

$head age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>&  %age_table%
%$$
and is the $cref/age_table/get_age_table/age_table/$$.

$head time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>&  %time_table%
%$$
and is the $cref/time_table/get_time_table/time_table/$$.

$head integrand_table$$
This argument has prototype
$codei%
	const CppAD::vector<integrand_struct>&  %integrand_table%
%$$
and is the $cref/integrand_table/get_integrand_table/integrand_table/$$.

$head node_table$$
This argument has prototype
$codei%
	const CppAD::vector<node_struct>& %node_table%
%$$
and is the $cref/node_table/get_node_table/node_table/$$.
Only the $code parent$$ field of this table is used.

$head data_table$$
This argument has prototype
$codei%
	const CppAD::vector<data_struct>&  %data_table%
%$$
and is the $cref/data_table/get_data_table/data_table/$$.

$head w_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<weight_info>& %w_info_vec%
%$$
For each $cref/weight_id/weight_table/weight_id/$$,
$codei%
	%w_info_vec%[ %weight_id% ]
%$$
is the corresponding $cref weight_info$$ information.

$head s_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info>& %s_info_vec%
%$$
For each $cref/smooth_id/smooth_table/smooth_id/$$,
$codei%
	%s_info_vec%[ %smooth_id% ]
%$$
is the corresponding $cref smooth_info$$ information.
For each $icode%s_info_vec%[%smooth_id%]%$$ object,
only the following functions are used:
$code age_size$$, $code time_size$$, $code age_id$$, $code time_id$$.

$head var_info$$
This argument has prototype
$codei%
	const pack_var& %var_info%
%$$
and is the $cref pack_var$$ information corresponding to corresponding
to this problem.

$head prior_table$$
This argument has prototype
$codei%
	const CppAD::vector<prior_struct>&  %prior_table%
%$$
and is the $cref/prior_table/get_prior_table/prior_table/$$.
Only to following fields are used
$cref/density_id/prior_table/density_id/$$,
$cref/mean/prior_table/mean/$$,
$cref/std/prior_table/std/$$,
$cref/eta/prior_table/eta/$$.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_at/include/joint_like.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

// consctructor
joint_like::joint_like(
	size_t                                 parent_node_id  ,
	size_t                                 n_age_ode       ,
	size_t                                 n_time_ode      ,
	double                                 ode_step_size   ,
	const CppAD::vector<double>&           age_table       ,
	const CppAD::vector<double>&           time_table      ,
	const CppAD::vector<integrand_struct>& integrand_table ,
	const CppAD::vector<node_struct>&      node_table      ,
	const CppAD::vector<data_struct>&      data_table      ,
	const CppAD::vector<weight_info>&      w_info_vec      ,
	const CppAD::vector<smooth_info>&      s_info_vec      ,
	const pack_var&                        var_info        ,
	const CppAD::vector<prior_struct>&     prior_table     )
:
dm_(
	parent_node_id  ,
	n_age_ode       ,
	n_time_ode      ,
	ode_step_size   ,
	age_table       ,
	time_table      ,
	integrand_table ,
	node_table      ,
	data_table      ,
	w_info_vec      ,
	s_info_vec
)
{ }


} // END DISMOD_AT_NAMESPACE
