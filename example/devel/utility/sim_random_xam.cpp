// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin sim_random_xam.cpp$$
$spell
	gsl
	rng
$$

$section Manage GSL Random Number Generator: Example and Test$$

$code
$verbatim%example/devel/utility/sim_random_xam.cpp%
	0%// BEGIN C++%// END C++%1%$$
$$

$end
*/
// BEGIN C++
# include <cmath>
# include <dismod_at/get_density_table.hpp>
# include <dismod_at/manage_gsl_rng.hpp>
# include <dismod_at/sim_random.hpp>

bool sim_random_xam(void)
{	bool ok = true;
	//
	size_t sample_size = 1000;
	//
	dismod_at::density_enum density;
	double mu, delta, eta, probability, check;
	size_t count;
	//
	// initialize random number generator using the clock
	dismod_at::new_gsl_rng(0);
	//
	// check Gausian
	density = dismod_at::gaussian_enum;
	mu      = 0.0;
	delta   = 1.0;
	eta     = 0.0; // not used (avoid warning)
	count   = 0;
	for(size_t i = 0; i < sample_size; i++)
	{	double z = dismod_at::sim_random(density, mu, delta, eta);
		if( 0.5 <= z )
			count ++;
	}
	probability = double(count) / double(sample_size);
	//
	// https://en.wikipedia.org/wiki/Standard_normal_table
	check = 0.5 - 0.19146;
	ok   &= std::fabs( probability / check - 1.0 ) < 1e-1;
	//
	// free random number generator
	dismod_at::free_gsl_rng();
	//
	return ok;
}
// END C++
