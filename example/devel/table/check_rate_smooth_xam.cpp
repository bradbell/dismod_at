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
$begin check_rate_smooth_xam.cpp$$
$spell
	xam
$$

$section C++ check_rate_smooth: Example and Test$$
$index example, C++ check_rate_smooth$$
$index check_rate_smooth, C++ example$$

$code
$verbatim%example/devel/table/check_rate_smooth_xam.cpp
%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
# include <dismod_at/include/check_rate_smooth.hpp>

bool check_rate_smooth_xam(void)
{	bool ok = true;
	size_t i, j;
	//
	size_t n_rate = size_t(dismod_at::number_rate_enum);
	CppAD::vector<dismod_at::rate_struct> rate_table(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	rate_table[rate_id].parent_smooth_id = 0;
		rate_table[rate_id].child_smooth_id  = 1;
	}
	//
	size_t n_age = 3;
	CppAD::vector<size_t> age_id(n_age);
	for(i = 0; i < n_age; i++)
		age_id[i] = 2 * i;
	//
	size_t n_time = 4;
	CppAD::vector<size_t> time_id(n_time);
	for(j = 0; j < n_time; j++)
		time_id[j] = 3 * j;
	//
	size_t n_grid = n_age * n_time;
	CppAD::vector<size_t> value_prior_id(n_grid);
	CppAD::vector<size_t> dage_prior_id (n_grid);
	CppAD::vector<size_t> dtime_prior_id(n_grid);
	size_t mulstd_value = 0;
	size_t mulstd_dage = 0;
	size_t mulstd_dtime = 0;
	//
	size_t n_smooth = 2;
	CppAD::vector<dismod_at::smooth_info*>  s_info_ptr(n_smooth);
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
	{	s_info_ptr[smooth_id] = new dismod_at::smooth_info(
			age_id, 
			time_id, 
			value_prior_id, 
			dage_prior_id, 
			dtime_prior_id,
			mulstd_value,
			mulstd_dage,
			mulstd_dtime
		);
		// uncomment line below to get an error message
		// --time_id[n_time - 1];
	}
	//
	dismod_at::check_rate_smooth(rate_table, s_info_ptr);
	//
	for(size_t smooth_id = 0; smooth_id < n_smooth; smooth_id++)
		delete s_info_ptr[smooth_id];
	//
	return ok;
}
