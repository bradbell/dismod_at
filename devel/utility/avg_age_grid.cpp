// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/avg_age_grid.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/split_space.hpp>
# include <dismod_at/time_line_vec.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
$begin avg_age_grid$$

$section Split the Average Age Grid$$

$head Syntax$$
$icode%age_grid% = avg_age_grid(
	%ode_step_size%, %avg_age_split%, %age_table%
)%$$

$head Prototype$$
$srcfile%devel/utility/avg_age_grid.cpp%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head ode_step_size$$
is the $cref/ode_step_size/option_table/ode_step_size/$$.

$head avg_age_split$$
is the $cref/avg_age_split/option_table/avg_age_split/$$.

$head age_table$$
is the $cref age_table$$.

$head age_grid$$
is the $cref/average age grid/option_table/avg_age_split/Average Age Grid/$$.

$children%example/devel/utility/avg_age_grid_xam.cpp
%$$
$head Example$$
The file $cref avg_age_grid_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
// BEGIN_PROTOTYPE
CppAD::vector<double> avg_age_grid(
	double                         ode_step_size  ,
	const std::string              avg_age_split  ,
	const CppAD::vector<double>&   age_table      )
// END_PROTOTYPE
{	using std::string;
	using CppAD::vector;
	//
	vector<double> result;

	// minimum and maximum age in age table
	double age_min    = dismod_at::min_vector( age_table );
	double age_max    = dismod_at::max_vector( age_table );
	//
	// number of points in the uniform spaced grid
	double eps        = std::numeric_limits<double>::epsilon();
	double two        = 2.0 - 100. * eps;
	size_t n_uniform  = size_t( (age_max - age_min)/ode_step_size + two );
	double s_uniform  = (age_max - age_min) / double(n_uniform - 1);
	//
	// split_vec_str
	vector<string> split_vec_str = dismod_at::split_space( avg_age_split );
	//
	// n_split
	size_t n_split = split_vec_str.size();
	//
	// split_vec
	vector<double> split_vec(n_split);
	for(size_t i = 0; i < n_split; ++i)
		split_vec[i] = std::atof( split_vec_str[i].c_str() );
	//
	if( n_split == 0 )
	{	result.resize(n_uniform);
		for(size_t i = 0; i < n_uniform; ++i)
			result[i] = age_min + double(i) * s_uniform;
	}
	else
	{	// put age_min in the result
		size_t split_index = 0;
		double split = split_vec[split_index];
		bool error   = split < age_min;
		error |= dismod_at::time_line_vec<double>::near_equal(split, age_min);
		if( error )
		{	string table_name = "option";
			string msg  = "avg_age_split entry ";
			msg += split_vec_str[split_index];
			msg += " is less than or equal minimum age in age table";
			error_exit(msg, table_name);
		}
		result.push_back( age_min );
		//
		// put age in the result
		for(size_t i = 1; i < n_uniform; ++i)
		{	double age  = age_min + double(i) * s_uniform;
			bool   near = time_line_vec<double>::near_equal(split, age);
			//
			// check if adding split to result
			if( ! near && split < age  )
				result.push_back( split );
			//
			// put age in result
			result.push_back( age );
			//
			// check if advance to next split
			if( near || split < age )
			{	// advance to next split point
				++split_index;
				if( split_index < n_split )
				{	age   = split;
					split = split_vec[split_index];
					if( split <= age )
					{	string table_name = "option";
						string msg  = "avg_age_split entry ";
						msg += split_vec_str[split_index];
						msg += " is less than or equal previous entry";
						error_exit(msg, table_name);
					}
					if( age_max <= split )
					{	string table_name = "option";
						string msg  = "avg_age_split entry ";
						msg += split_vec_str[split_index];
						msg += " >= the maximum age in age table";
						error_exit(msg, table_name);
					}
				}
				else
					split = age_max;
			}
		}
	}
	return result;
}

} // END_DISMOD_AT_NAMESPACE
