// $Id:$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
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

$srcthisfile%
	0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <cmath>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_density_table.hpp>
# include <cppad/mixed/manage_gsl_rng.hpp>
# include <dismod_at/sim_random.hpp>
# include <gsl/gsl_cdf.h>

bool sim_random_xam(void)
{	bool ok = true;
	//
	//
	// Initialize random number generator using the clock
	size_t random_seed = CppAD::mixed::new_gsl_rng(1436703881);
	//
	size_t sample_size = 100*100*100;
	// -------------------------------------------------------------------
	// check Gausian
	bool difference                 = false; // does not matter
	dismod_at::density_enum density = dismod_at::gaussian_enum;
	double delta   = 0.5;
	double mu      = 0.0;
	size_t count   = 0;
	double sum_z   = 0.0;
	double sum_zsq = 0.0;
	double eta     = 0.0; // not used (avoid warning)
	double nu      = 0.0; // not used (avoid warning)
	for(size_t i = 0; i < sample_size; i++)
	{	double z = dismod_at::sim_random(
			difference, density, mu, mu, delta, eta, nu
		);
		if( 0.5 <= z / delta )
			count ++;
		sum_z   += z;
		sum_zsq += z * z;
	}
	double samp_mean = sum_z / double(sample_size);
	double samp_var  = sum_zsq / double(sample_size);
	double samp_prob = double(count) / double(sample_size);
	//
	double check = 0.0;
	ok   &= std::fabs(samp_mean - mu) < 2.0 / std::sqrt( double(sample_size) );
	check = delta * delta;
	ok   &= std::fabs(samp_var / check - 1.0) < 1e-1;
	// https://en.wikipedia.org/wiki/Standard_normal_table
	check = 0.5 - 0.19146;
	ok   &= std::fabs( samp_prob / check - 1.0 ) < 1e-1;
	// -------------------------------------------------------------------
	// check Laplace
	density = dismod_at::laplace_enum;
	delta   = 0.5;
	mu      = 0.0;
	count   = 0;
	sum_z   = 0.0;
	sum_zsq = 0.0;
	for(size_t i = 0; i < sample_size; i++)
	{	difference = bool( i % 2 ); // does not matter
		double z = dismod_at::sim_random(
			difference, density, mu, mu, delta, eta, nu
		);
		if( 0.5 <= z / delta )
			count ++;
		sum_z   += z;
		sum_zsq += z * z;
	}
	samp_mean = sum_z / double(sample_size);
	samp_var  = sum_zsq / double(sample_size);
	samp_prob = double(count) / double(sample_size);
	//
	check = 0.0;
	ok   &= std::fabs(samp_mean - mu) < 2.0 / std::sqrt( double(sample_size) );
	check = delta * delta;
	ok   &= std::fabs(samp_var / check - 1.0) < 1e-1;
	//
	// integral from 0.5 to infinity of p(z) dz
	check = exp( - std::sqrt(2.0) * 0.5 ) / 2.0;
	ok   &= std::fabs( samp_prob / check - 1.0 ) < 1e-1;
	// -------------------------------------------------------------------
	// check Student's-t
	density = dismod_at::students_enum;
	delta   = 0.5;
	mu      = 0.0;
	count   = 0;
	sum_z   = 0.0;
	sum_zsq = 0.0;
	nu      = 7.0;
	for(size_t i = 0; i < sample_size; i++)
	{	difference = bool( i % 2 ); // does not matter
		double z = dismod_at::sim_random(
			difference, density, mu, mu, delta, eta, nu
		);
		if( 0.5 <= z / delta )
			count ++;
		sum_z   += z;
		sum_zsq += z * z;
	}
	samp_mean = sum_z / double(sample_size);
	samp_var  = sum_zsq / double(sample_size);
	samp_prob = double(count) / double(sample_size);
	//
	check = 0.0;
	ok   &= std::fabs(samp_mean - mu) < 2.0 / std::sqrt( double(sample_size) );
	check = delta * delta;
	ok   &= std::fabs(samp_var / check - 1.0) < 1e-1;
	//
	// integral from 0.5 to infinity of p(z) dz
	check = gsl_cdf_tdist_Q(0.5, nu);
	ok   &= std::fabs( samp_prob / check - 1.0 ) < 1e-1;
	// -------------------------------------------------------------------
	// check Log-Gausian
	density        = dismod_at::log_gaussian_enum;
	delta          = 1e-1;
	mu             = 1e-1;
	eta            = 1e-2;
	count          = 0;
	double sum_w   = 0.0;
	double sum_wsq = 0.0;
	double sigma   = log(mu + eta + delta) - log(mu + eta);
	for(size_t i = 0; i < sample_size; i++)
	{	difference   = bool( i % 2 ); // does matter
		double z     = dismod_at::sim_random(
			difference, density, mu, mu, delta, eta, nu
		);
		double w = ( log(z + eta) - log(mu + eta) ) / sigma;
		if( difference )
			w    = ( log(z + eta) - log(mu + eta) ) / delta;

		if( 0.5 <= w )
			count ++;
		sum_w   += w;
		sum_wsq += w * w;
	}
	samp_mean = sum_w / double(sample_size);
	samp_var  = sum_wsq / double(sample_size);
	samp_prob = double(count) / double(sample_size);
	//
	check = 0.0;
	ok   &= std::fabs(samp_mean) < 2.0 / std::sqrt( double(sample_size) );
	ok   &= std::fabs(samp_var - 1.0) < 1e-1;
	// https://en.wikipedia.org/wiki/Standard_normal_table
	check = 0.5 - 0.19146;
	ok   &= std::fabs( samp_prob / check - 1.0 ) < 1e-1;
	// -------------------------------------------------------------------
	// check Log-Laplace
	density = dismod_at::log_laplace_enum;
	delta   = 1e-1;
	mu      = 1e-1;
	eta     = 1e-2;
	count   = 0;
	sum_w   = 0.0;
	sum_wsq = 0.0;
	sigma   = log(mu + eta + delta) - log(mu + eta);
	for(size_t i = 0; i < sample_size; i++)
	{	difference   = bool( i % 2 ); // does matter
		double z     = dismod_at::sim_random(
			difference, density, mu, mu, delta, eta, nu
		);
		double w     = ( log(z + eta) - log(mu + eta) ) / sigma;
		if( difference )
			w    = ( log(z + eta) - log(mu + eta) ) / delta;
		if( 0.5 <= w )
			count ++;
		sum_w   += w;
		sum_wsq += w * w;
	}
	samp_mean = sum_w / double(sample_size);
	samp_var  = sum_wsq / double(sample_size);
	samp_prob = double(count) / double(sample_size);
	//
	check = 0.0;
	ok   &= std::fabs(samp_mean) < 3.0 / std::sqrt( double(sample_size) );
	ok   &= std::fabs(samp_var - 1.0) < 1e-1;
	// integral from 0.5 to infinity of p(z) dz
	check = exp( - std::sqrt(2.0) * 0.5 ) / 2.0;
	// std::cout << samp_prob / check << std::endl;
	ok   &= std::fabs( samp_prob / check - 1.0 ) < 1e-1;
	// -------------------------------------------------------------------
	// check Log-Student's-t
	density = dismod_at::log_students_enum;
	delta   = 1e-1;
	mu      = 1e-1;
	eta     = 1e-2;
	nu      = 7.0;
	count   = 0;
	sum_w   = 0.0;
	sum_wsq = 0.0;
	sigma   = log(mu + eta + delta) - log(mu + eta);
	for(size_t i = 0; i < sample_size; i++)
	{	difference   = bool( i % 2 ); // does matter
		double z     = dismod_at::sim_random(
			difference, density, mu, mu, delta, eta, nu
		);
		double w     = ( log(z + eta) - log(mu + eta) ) / sigma;
		if( difference )
			w    = ( log(z + eta) - log(mu + eta) ) / delta;
		if( 0.5 <= w )
			count ++;
		sum_w   += w;
		sum_wsq += w * w;
	}
	samp_mean = sum_w / double(sample_size);
	samp_var  = sum_wsq / double(sample_size);
	samp_prob = double(count) / double(sample_size);
	//
	check = 0.0;
	ok   &= std::fabs(samp_mean) < 3.0 / std::sqrt( double(sample_size) );
	ok   &= std::fabs(samp_var - 1.0) < 1e-1;
	// integral from 0.5 to infinity of p(z) dz
	check = gsl_cdf_tdist_Q(0.5, nu);
	// std::cout << samp_prob / check << std::endl;
	ok   &= std::fabs( samp_prob / check - 1.0 ) < 1e-1;
	// ---------------------------------------------------------------------
	if( ! ok )
		std::cout << "random_seed = " << random_seed << std::endl;
	// ---------------------------------------------------------------------
	// free random number generator
	CppAD::mixed::free_gsl_rng();
	//
	return ok;
}
// END C++
