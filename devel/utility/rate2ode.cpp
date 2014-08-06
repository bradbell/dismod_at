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

$section Interpolating the Rates onto the ODE grid$$

$head Syntax$$
$codei%rate2ode %r2ode%(
	%n_age_ode%, %n_time_ode%, %ode_step_size% , %age_table%, %time_table%, 
	%s_info_vec%, %rate_table%
)%$$
$codei%
%rate_ode% = %r2ode%.interpolate(%rate_id%, %rate_si%, %ode_index% )%$$

$head Float$$
The type $icode Float$$ must be one of the following:
$code double$$, $code CppAD::AD<double>$$

$head rate2ode$$
This constructs an object that interpolates from 
the specified smoothing grid to the ode grid.

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

$subhead s_info_vec$$
This argument has prototype
$codei%
	const CppAD::vector<smooth_info>& %s_info_vec%
%$$
and its size is equal to $icode n_smooth$$ the number of rows in the
$cref/smooth/smooth_table/$$ table.
For $icode%smooth_id% = 0 , %...%, %s_info_vec%.size()%$$,
we use the following notation:
$codei%
	%si%                    = %smooth_id%
	%n_age%(%si%)           = %s_info_vec%[%smooth_id%].age_size()
	%n_time%(%si%)          = %s_info_vec%[%smooth_id%].time_size()
	%age_id%(%si%, %i_si%)  = %s_info_vec%[%smooth_id%].age_id(%i_si%)
	%time_id%(%si%, %j_si%) = %s_info_vec%[%smooth_id%].time_id(%j_si%)
%$$
where $icode%i_si% < %n_age%(%si%)%$$ and
$icode%j_si% < %n_time%(%si%)%$$.
Note that these are the only functions of
$icode%s_info_vec%[%smooth_id%]%$$ that are used.

$head rate_table$$
This argument has prototype
$codei%
	CppAD::vector<rate_struct>& %rate_table
%$$
and is the 
$cref/rate_table/get_rate_table/rate_table/$$.

$head interpolate$$
This is a $code const$$ function 
that interpolates rates from thier smoothing grid to the ode grid.

$subhead rate_id$$
This argument has prototype
$codei%
	size_t %rate_id%
%$$
and is the $cref/rate_id/rate_table/rate_id/$$ for the 
rate that we are interpolating.

$subhead rate_si$$
This argument has prototype
$codei%
	const CppAD::vector<%Float%>& %rate_si%
%$$
and its size is $icode%n_age%(%si%)%*%n_time%(%si%)%$$ where 
$codei%
	%si% = %rate_table%[%rate_id%].smooth_id
%$$
For $icode%i_si% = 0 , %...%, %n_age%(%si%)-1%$$,
$icode%j_si% = 0 , %...%, %n_time%(%si%)-1%$$,
$codei%
	%rate_si%[ %i_si% * %n_time%(%si%) + %j_si% ]
%$$
is the value of the variable of the rate being interpolated at
$codei%
	%age%  =  %age_table%[  %age_id%(%si%, %i_si%) ] 
	%time% = %time_table%[ %time_id%(%si%, %j_si%) ] 
%$$

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

$subhead rate_ode$$
This return value has prototype
$codei%
	const CppAD::vector<%Float%>& %rate_ode%
%$$
and its size is $icode%ode_index%.size()%$$.
For $icode%k% = 0 , %...%, %ode_index%.size()-1%$$,
$codei%
	%rate_ode%[ %k% ]
%$$
is the interpolated value for the rate at
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
	const CppAD::vector<smooth_info>&           s_info_vec    ,
	const CppAD::vector<rate_struct>&           rate_table    )
{	size_t n_smooth = s_info_vec.size();

	// s2ode_ptr_
	s2ode_ptr_.resize( n_smooth );
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	const smooth_info& s_info = s_info_vec[smooth_id];
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
		const smooth_info& s_info_parent = s_info_vec[parent_smooth_id];
		const smooth_info& s_info_child  = s_info_vec[child_smooth_id];

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
