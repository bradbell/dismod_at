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
$begin bilinear_interp_xam.cpp$$
$spell
	interp
	xam
$$

$section C++ bilinear_interp: Example and Test$$
$index example, C++ bilinear_interp$$
$index bilinear_interp, C++ example$$

$code
$verbatim%example/devel/model/bilinear_interp_xam.cpp
	%0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <limits>
# include <dismod_at/dismod_at.hpp>

bool bilinear_interp_xam(void)
{
	bool   ok = true;
	size_t i, j;
	using  std::string;
	using  CppAD::vector;	

	double eps = std::numeric_limits<double>::epsilon();

	size_t n_x = 4, n_y = 3;
	vector<double> x_grid(n_x), y_grid(n_y), z_grid(n_x * n_y);
	for(i = 0; i < n_x; i++)
		x_grid[i] = 10.0 * i;
	for(j = 0; j < n_y; j++)
		y_grid[j] = 1990.0 + 10 * j;
	for(i = 0; i < n_x; i++)
	{	for(j = 0; j < n_y; j++)
			z_grid[i * n_y + j] = x_grid[i] * y_grid[j] + 1.0;
	}

	// point at which to interpolate the function
	double x = 10.0 * 2.5;
	double y = 1990.0 + 10 * 1.5;
	i = 0;
	j = 0;
	// evaluate the interpolant
	double z = dismod_at::bilinear_interp(
		x, y, x_grid, y_grid, z_grid, i, j
	);
	ok = x_grid[i] <= x && x <= x_grid[i+1];
	ok = y_grid[j] <= y && y <= y_grid[j+1];

	double z_check  = x * y + 1.0;
	ok = std::fabs( 1.0 - z / z_check  ) < 10.0 * eps;

	return ok;
}
// END C++
