// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin age_avg_grid_xam.cpp$$
$spell
	xam
$$

$section C++ age_avg_grid: Example and Test$$

$srcfile%example/devel/utility/age_avg_grid_xam.cpp%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/age_avg_grid.hpp>

bool age_avg_grid_xam(void)
{	using CppAD::vector;

	bool   ok = true;

	double         ode_step_size = 20.0;
	std::string    age_avg_split = "25.0 35.0 50";
	vector<double> age_table(2);
	age_table[0] = 0.0;
	age_table[1] = 100.0;

	vector<double> age_avg_grid = dismod_at::age_avg_grid(
		ode_step_size, age_avg_split, age_table
	);

	ok &= age_avg_grid.size() == 9;
	double check[] = {
		0.0, 20.0, 25.0, 35.0, 40.0, 50.0, 60.0, 80.0, 100.0
	};
	// std::cout << "age_avg_grid = " << age_avg_grid << "\n";
	for(size_t i = 0; i < age_avg_grid.size(); ++i)
		ok &= age_avg_grid[i] == check[i];

	return ok;
}
// END C++
