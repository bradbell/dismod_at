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
	//
	// rate_table
	size_t n_rate = size_t(dismod_at::number_rate_enum);
	CppAD::vector<dismod_at::rate_struct> rate_table(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	if( rate_id == dismod_at::pini_enum )
		{	rate_table[rate_id].parent_smooth_id = 0;
			rate_table[rate_id].child_smooth_id  = 1;
		}
		else
		{	rate_table[rate_id].parent_smooth_id = 2;
			rate_table[rate_id].child_smooth_id  = 3;
		}
	}
	//
	// smooth_table
	size_t n_smooth = 4;
	CppAD::vector<dismod_at::smooth_struct> smooth_table(n_smooth);
	smooth_table[0].n_age = 1;
	smooth_table[1].n_age = 1;
	smooth_table[2].n_age = 2;
	smooth_table[3].n_age = 2;
	//
	dismod_at::check_rate_smooth(rate_table, smooth_table);
	//
	return ok;
}
