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
$begin avg_age_grid_xam.cpp$$
$spell
	xam
$$

$section C++ avg_age_grid: Example and Test$$

$code
$srcfile%example/devel/utility/avg_age_grid_xam.cpp%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <dismod_at/avg_age_grid.hpp>

bool avg_age_grid_xam(void)
{	using CppAD::vector;

	bool   ok = true;

	double         ode_step_size = 20.0;
	std::string    avg_age_split = "50.0 70.0";
	vector<double> age_table(2);
	age_table[0] = 0.0;
	age_table[1] = 100.0;

	vector<double> avg_age_grid = dismod_at::avg_age_grid(
		ode_step_size, avg_age_split, age_table
	);

	ok &= avg_age_grid.size() == 8;
	double check[] = {
		0.0, 20.0, 40.0, 50.0, 60.0, 70.0, 80.0, 100.0
	};
	// std::cout << "avg_age_grid = " << avg_age_grid << "\n";
	for(size_t i = 0; i < avg_age_grid.size(); ++i)
		ok &= avg_age_grid[i] == check[i];

	return ok;
}
// END C++
