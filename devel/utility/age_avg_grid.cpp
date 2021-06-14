// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# include <dismod_at/age_avg_grid.hpp>
# include <dismod_at/error_exit.hpp>
# include <dismod_at/min_max_vector.hpp>
# include <dismod_at/split_space.hpp>
# include <dismod_at/time_line_vec.hpp>

namespace dismod_at { // BEGIN_DISMOD_AT_NAMESPACE
/*
$begin age_avg_grid$$

$section Split the Age Average Grid$$

$head Syntax$$
$icode%age_grid% = age_avg_grid(
	%ode_step_size%, %age_avg_split%, %age_table%
)%$$

$head Prototype$$
$srcthisfile%
	0%// BEGIN_PROTOTYPE%// END_PROTOTYPE%1
%$$

$head ode_step_size$$
is the $cref/ode_step_size/option_table/ode_step_size/$$.

$head age_avg_split$$
is the $cref/age_avg_split/option_table/age_avg_split/$$.

$head age_table$$
is the $cref age_table$$.

$head age_grid$$
is the $cref/age average grid/age_avg_table/Age Average Grid/$$.

$children%example/devel/utility/age_avg_grid_xam.cpp
%$$
$head Example$$
The file $cref age_avg_grid_xam.cpp$$ contains an example and test
of using this routine.

$end
*/
// BEGIN_PROTOTYPE
CppAD::vector<double> age_avg_grid(
	double                         ode_step_size  ,
	const std::string              age_avg_split  ,
	const CppAD::vector<double>&   age_table      )
// END_PROTOTYPE
{	using std::string;
	using CppAD::vector;
	//
	vector<double> result;

	// minimum and maximum age in age table
	double age_min  = dismod_at::min_vector( age_table );
	double age_max  = dismod_at::max_vector( age_table );
	bool   near     = time_line_vec<double>::near_equal(age_min, age_max);
	if( near )
	{	string table_name = "age";
		string msg  = "minimum and maximum age are near equal";
		error_exit(msg, table_name);
	}
	//
	// number of points in the uniform spaced grid
	double eps        = std::numeric_limits<double>::epsilon();
	double two        = 2.0 - 100. * eps;
	size_t n_uniform  = size_t( (age_max - age_min)/ode_step_size + two );
	double s_uniform  = (age_max - age_min) / double(n_uniform - 1);
	//
	// split_vec_str
	vector<string> split_vec_str = dismod_at::split_space( age_avg_split );
	//
	// n_split
	size_t n_split = split_vec_str.size();
	//
	// split_vec
	vector<double> split_vec(n_split);
	for(size_t i = 0; i < n_split; ++i)
		split_vec[i] = std::atof( split_vec_str[i].c_str() );
	//
	// next value split age grid at
	size_t split_index = 0;
	double split       = 2.0 * age_max;
	if( split_index < n_split )
	{	split      = split_vec[split_index];
		bool error  = split < age_min;
		error |= dismod_at::time_line_vec<double>::near_equal(split, age_min);
		if( error )
		{	string table_name = "option";
			string msg  = "age_avg_split entry ";
			msg += split_vec_str[split_index];
			msg += " is less than or equal minimum age in age table";
			error_exit(msg, table_name);
		}
	}
	//
	// put age in the result
	for(size_t i = 0; i < n_uniform; ++i)
	{	double age  = age_min + double(i) * s_uniform;
		near        = time_line_vec<double>::near_equal(split, age);
		//
		// check if adding next split to result
		while( near || split < age  )
		{	if( ! near )
				result.push_back( split );
			//
			// advance to next split point
			++split_index;
			if( split_index < n_split )
			{	double previous = split;
				split           = split_vec[split_index];
				if( split <= previous )
				{	string table_name = "option";
					string msg  = "age_avg_split entry ";
					msg += split_vec_str[split_index];
					msg += " is less than or equal previous entry";
					error_exit(msg, table_name);
				}
				if( age_max <= split )
				{	string table_name = "option";
					string msg  = "age_avg_split entry ";
					msg += split_vec_str[split_index];
					msg += " >= the maximum age in age table";
					error_exit(msg, table_name);
				}
			}
			else
				split = 2.0 * age_max;
			//
			near = time_line_vec<double>::near_equal(split, age);
		}
		// add this point in the uniformly spaced grid
		result.push_back( age );
	}
	return result;
}

} // END_DISMOD_AT_NAMESPACE
