// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */

# include <limits>
# include <cmath>
# include <dismod_at/residual_density.hpp>

bool residual_density_xam(void)
{	bool ok    = true;
	double nan = std::numeric_limits<double>::quiet_NaN();
	double eps = 100. * std::numeric_limits<double>::epsilon();
	double pi  = 4.0 * std::atan(1.0);

	using std::sqrt;
	using std::fabs;
	dismod_at::density_enum density;
	dismod_at::residual_density_struct<double> wres_logden;
	double wres, logden, sigma, tmp;

	double z     = 1.5;
	double mu    = 2.0;
	double delta = 2.5;
	double eta   = nan;

	// uniform
	density     = dismod_at::uniform_enum;
	wres_logden = residual_density(density, z, mu, delta, eta);
	wres        = (z - mu) / delta;
	ok         &= fabs(1.0 - wres_logden.wres / wres) <= eps;
	ok         &= wres_logden.logden_smooth  == 0.0;
	ok         &= wres_logden.logden_sub_abs == 0.0;

	// gaussian
	density     = dismod_at::gaussian_enum;
	wres_logden = residual_density(density, z, mu, delta, eta);
	wres        = (z - mu) / delta;
	logden      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	ok         &= fabs(1.0 - wres_logden.wres / wres) <= eps;
	ok         &= fabs(1.0 - wres_logden.logden_smooth / logden) <= eps;
	ok         &= wres_logden.logden_sub_abs == 0.0;

	// laplace
	density     = dismod_at::laplace_enum;
	wres_logden = residual_density(density, z, mu, delta, eta);
	wres        = (z - mu) / delta;
	logden      = - log(delta * sqrt(2.0) ) - sqrt(2.0) * fabs( wres );
	ok         &= fabs(1.0 - wres_logden.wres / wres) <= eps;
	tmp         = wres_logden.logden_smooth;
	tmp        -= fabs( wres_logden.logden_sub_abs );
	ok         &= fabs(1.0 - tmp / logden) <= eps;

	// log-gaussian
	density     = dismod_at::log_gaussian_enum;
	eta         = 3.0;
	wres_logden = residual_density(density, z, mu, delta, eta);
	sigma       = log(mu + eta + delta) - log(mu + eta); 
	wres        = ( log(z + eta) - log(mu + eta) ) / sigma;
	logden      = - log(sigma * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	ok         &= fabs(1.0 - wres_logden.wres / wres) <= eps;
	ok         &= fabs(1.0 - wres_logden.logden_smooth / logden) <= eps;
	ok         &= wres_logden.logden_sub_abs == 0.0;

	// log-laplace
	density     = dismod_at::log_laplace_enum;
	eta         = 3.0;
	wres_logden = residual_density(density, z, mu, delta, eta);
	sigma       = log(mu + eta + delta) - log(mu + eta); 
	wres        = ( log(z + eta) - log(mu + eta) ) / sigma;
	logden      = - log(sigma * sqrt(2.0) ) - sqrt(2.0) * fabs( wres );
	ok         &= fabs(1.0 - wres_logden.wres / wres) <= eps;
	tmp         = wres_logden.logden_smooth;
	tmp        -= fabs( wres_logden.logden_sub_abs );
	ok         &= fabs(1.0 - tmp / logden) <= eps;

	return ok;
}
