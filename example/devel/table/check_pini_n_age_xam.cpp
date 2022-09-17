// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin check_pini_n_age_xam.cpp$$
$spell
	xam
$$

$section C++ check_pini_n_age: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
# include <dismod_at/check_pini_n_age.hpp>
# include <dismod_at/open_connection.hpp>
# include <dismod_at/null_int.hpp>

bool check_pini_n_age_xam(void)
{	bool ok = true;
	//
	// database
	bool new_file = true;
	std::string file_name = "example.db";
	sqlite3* db           = dismod_at::open_connection(file_name, new_file);
	//
	// rate_table
	size_t n_rate = size_t(dismod_at::number_rate_enum);
	CppAD::vector<dismod_at::rate_struct> rate_table(n_rate);
	for(size_t rate_id = 0; rate_id < n_rate; rate_id++)
	{	if( rate_id == dismod_at::pini_enum )
		{	rate_table[rate_id].parent_smooth_id = 0;
			rate_table[rate_id].child_smooth_id  = 1;
			rate_table[rate_id].child_nslist_id  = DISMOD_AT_NULL_INT;
		}
		else
		{	rate_table[rate_id].parent_smooth_id = 2;
			rate_table[rate_id].child_smooth_id  = 3;
			rate_table[rate_id].child_nslist_id  = DISMOD_AT_NULL_INT;
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
	dismod_at::check_pini_n_age(db, rate_table, smooth_table);
	//
	return ok;
}
