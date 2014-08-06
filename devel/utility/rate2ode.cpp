// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin rate2ode$

$section Computing the Rates on the ODE grid$$

$head Syntax$$
$codei%rate2ode %rate_ode%(
	%n_age_ode%, %n_time_ode%, %ode_step_size% , %age_table%, %time_table%, 
	%s_info_ptr%, %rate_table%
)%$$
$codei%
%rate% = %rate_ode%.compute(
	%rate_id%, %data_id%, %var_info%, %var_vec%, %ode_index%
)%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head rate2ode$$
This constructs an object that computes the rates on the
ode grid.

$subhead n_age_ode$$
This argument has prototype
$codei%
	size_t %n_age_ode%
%$$
It is the number of points in the
$cref/ode age grid/glossary/Ode Grid/Age, a_i/$$.

$subhead n_time_ode$$
This argument has prototype
$codei%
	size_t %n_time_ode%
%$$
It is the number of points in the
$cref/ode time grid/glossary/Ode Grid/Time, t_j/$$.

$subhead ode_step_size$$
This argument has prototype
$codei%
	double %ode_step_size%
%$$
and is the value of $cref/ode_step_size/run_table/ode_step_size/$$
in the run table.

$subhead age_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %age_table%
%$$
and is the age values corresponding to the $icode age_id$$ values.

$subhead time_table$$
This argument has prototype
$codei%
	const CppAD::vector<double>& %time_table%
%$$
and is the time values corresponding to the $icode time_id$$ values.

$head s_info_ptr$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info*> %smooth_info_ptr%
%$$
Let $icode n_smooth$$ be the number of rows in the
$cref smooth_table$$.
For $icode%smooth_id% = 0 , %...%, %n_smooth%-1%$$ the value
$codei%
	*(%smooth_info_ptr%[%smooth_id%])
%$$
is the $cref smooth_info$$ information for the corresponding
$cref/smooth_id/smooth_grid_table/smooth_id/$$.
For $icode%smooth_id% = 0 , %...%, %n_smooth%-1%$$,
we use the following notation:
$codei%
	%si%                    = %smooth_id%
	%n_age%(%si%)           = %s_info_ptr%[%smooth_id%]->age_size()
	%n_time%(%si%)          = %s_info_ptr%[%smooth_id%]->time_size()
	%age_id%(%si%, %i_si%)  = %s_info_ptr%[%smooth_id%]->age_id(%i_si%)
	%time_id%(%si%, %j_si%) = %s_info_ptr%[%smooth_id%]->time_id(%j_si%)
%$$
where $icode%i_si% < %n_age%(%si%)%$$ and
$icode%j_si% < %n_time%(%si%)%$$.
Note that these are the only functions of
$icode%s_info_ptr%[%smooth_id%]%$$ that are used.

$head rate_table$$
This argument has prototype
$codei%
	CppAD::vector<rate_struct>& %rate_table
%$$
and is the 
$cref/rate_table/get_rate_table/rate_table/$$.

$head compute$$
This is a $code const$$ function 
that computes the value for one rate on a speicified set of ode grid points.

$subhead rate_id$$
This argument has prototype
$codei%
	size_t %rate_id%
%$$
and is the $cref/rate_id/rate_table/rate_id/$$ for the 
rate that we are computing.

$subhead var_info$$
This argument has prototype
$codei%
	const var_pack& var_info
%$$
and is the $cref var_pack$$ information for this model.

$subhead var_vec$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %var_vec%
%$$
and is the value for all the variables in the model,
in the order and sizes prescribed by $icode var_info$$.

$subhead ode_index$$
This argument has prototype
$codei%
	const CppAD::vector<size_t>& %ode_index%
%$$
For $icode%k% = 0 , %...%, %ode_index%.size()%$$
the corresponding ode grid point age
$cref/a_i/glossary/Ode Grid/Age, a_i/$$ is defined by
$codei%
	%i%(%k%) = int( %ode_index%[%k%] / %n_time_ode% )
%$$
and the ode grid point time
$cref/t_i/glossary/Ode Grid/Time, t_j/$$ is defined by
$codei%
	%j%(%k%) = mod( %ode_index%[%k%], %n_time_ode% )
%$$

$subhead rate$$
This return value has prototype
$codei%
	const CppAD::vector<%Float%>& %rate%
%$$
and its size is $icode%ode_index%.size()%$$.
For $icode%k% = 0 , %...%, %ode_index%.size()-1%$$,
$codei%
	%rate%[ %k% ]
%$$
is the computed value for the rate at
$codei%
	%age%  = %a_i% =  %age_table%[0] + %i%(%k%) * %ode_step_size%
	%time% = %t_j% = %time_table%[0] + %j%(%k%) * %ode_step_size%
%$$

$children%example/devel/utility/rate2ode.cpp
%$$
$head Example$$
The file $cref rate2ode.cpp$$ contains an example and test
of using this routine.

$end
-----------------------------------------------------------------------------
*/
# include <dismod_ode/include/rate2ode.hpp>

namespace dismod_at { // BEGIN DISMOD_AT_NAMESPACE

rate2ode::rate2ode(
	size_t                                      n_age_ode     ,
	size_t                                      n_time_ode    ,
	double                                      ode_step_size ,
	const CppAD::vector<double>&                age_table     ,
	const CppAD::vector<double>&                time_table    ,
	const CppAD::vector<smooth_info>&           s_info_ptr    ,
	const CppAD::vector<rate_struct>&           rate_table    )
{	size_t n_smooth = s_info_ptr.size();

	// s2ode_ptr_
	s2ode_ptr_.resize( n_smooth );
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	const smooth_info& s_info = s_info_ptr[smooth_id];
		s2ode_ptr[smooth_id] = new smooth2ode(
			n_age_ode, n_time_ode, ode_step_size, age_table, time_table,
			s_info
		);
	}

	// rate_id2smooth_id_
	rate_id2smooth_id_.resize(number_rate_enum);
	for(size_t rate_id = 0; rate_id < number_rate_enum; rate_id++)
	{	size_t parent_smooth_id    = rate_table[rate_id].parent_smooth_id;
		rate_id2smooth_id[rate_id] = parent_smooth_id;

		size_t child_smooth_id  = rate_table[rate_id].child_smooth_id;
		const smooth_info& s_info_parent = s_info_ptr[parent_smooth_id];
		const smooth_info& s_info_child  = s_info_ptr[child_smooth_id];

		// check that, as far as smooth_info is concerned,
		// child_smooth_id is same as parent_smooth_id
		size_t n_age_parent     = s_info_parent.age_size();
		size_t n_age_child      = s_info_child.age_size();
		string message;
		if( n_age_parent != n_age_child )
		{	message = "parent_smooth_id and child_smooth_id"
			" correspond to different n_age in smooth table";
			table_error_exit("rate", rate_id, message);
		}	
		for(size_t i_si = 0; i_si < n_age_child; i_si++)
		{	size_t age_id_parent = s_info_parent.age_id(i_is);
			size_t age_id_child  = s_info_child.age_id(i_is);
			if( age_id_parent != age_id_child )
			{	message = "parent_smooth_id and child_smooth_id"
				" correpsond to different age_id values in smooth table";
				table_error_exit("rate", rate_id, message);
			}	
			
	}
}
rate2ode::~rate2ode(void)
{	size_t n_smooth = s2ode_ptr.size();
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
		delete s2ode_ptr[smooth_id];
}


} // END DISMOD_AT_NAMESPACE
