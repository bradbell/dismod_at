// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
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
	dismod_at::residual_struct<double> residual;
	double wres, logden, sigma, tmp;

	double z     = 1.5;
	double mu    = 2.0;
	double delta = 2.5;
	double eta   = nan;

	// uniform
	density     = dismod_at::uniform_enum;
	residual    = residual_density(density, z, mu, delta, eta);
	wres        = (z - mu) / delta;
	ok         &= fabs(1.0 - residual.wres / wres) <= eps;
	ok         &= residual.logden_smooth  == 0.0;
	ok         &= residual.logden_sub_abs == 0.0;
	ok         &= residual.density == density;

	// gaussian
	density     = dismod_at::gaussian_enum;
	residual    = residual_density(density, z, mu, delta, eta);
	wres        = (z - mu) / delta;
	logden      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	ok         &= fabs(1.0 - residual.wres / wres) <= eps;
	ok         &= fabs(1.0 - residual.logden_smooth / logden) <= eps;
	ok         &= residual.logden_sub_abs == 0.0;
	ok         &= residual.density == density;

	// laplace
	density     = dismod_at::laplace_enum;
	residual    = residual_density(density, z, mu, delta, eta);
	wres        = (z - mu) / delta;
	logden      = - log(delta * sqrt(2.0) ) - sqrt(2.0) * fabs( wres );
	ok         &= fabs(1.0 - residual.wres / wres) <= eps;
	tmp         = residual.logden_smooth;
	tmp        -= fabs( residual.logden_sub_abs );
	ok         &= fabs(1.0 - tmp / logden) <= eps;
	ok         &= residual.density == density;

	// log-gaussian
	density     = dismod_at::log_gaussian_enum;
	eta         = 3.0;
	residual    = residual_density(density, z, mu, delta, eta);
	sigma       = log(mu + eta + delta) - log(mu + eta);
	wres        = ( log(z + eta) - log(mu + eta) ) / sigma;
	logden      = - log(sigma * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	ok         &= fabs(1.0 - residual.wres / wres) <= eps;
	ok         &= fabs(1.0 - residual.logden_smooth / logden) <= eps;
	ok         &= residual.logden_sub_abs == 0.0;
	ok         &= residual.density == density;

	// log-laplace
	density     = dismod_at::log_laplace_enum;
	eta         = 3.0;
	residual    = residual_density(density, z, mu, delta, eta);
	sigma       = log(mu + eta + delta) - log(mu + eta);
	wres        = ( log(z + eta) - log(mu + eta) ) / sigma;
	logden      = - log(sigma * sqrt(2.0) ) - sqrt(2.0) * fabs( wres );
	ok         &= fabs(1.0 - residual.wres / wres) <= eps;
	tmp         = residual.logden_smooth;
	tmp        -= fabs( residual.logden_sub_abs );
	ok         &= fabs(1.0 - tmp / logden) <= eps;
	ok         &= residual.density == density;

	return ok;
}
