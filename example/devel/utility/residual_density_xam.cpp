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

namespace {
	bool check(
		const dismod_at::residual_struct<double>& residual       ,
		double                                    wres           ,
		double                                    smooth         ,
		double                                    sub_abs        ,
		dismod_at::density_enum                   d              ,
		bool                                      difference     )
	{	bool ok    = true;
		double eps = 100. * std::numeric_limits<double>::epsilon();
		ok &= fabs(1.0 - residual.wres / wres) <= eps;
		ok &= fabs(1.0 - residual.logden_smooth / smooth) <= eps;
		if( sub_abs == 0.0 )
			ok &= residual.logden_sub_abs == 0.0;
		else
			ok &= fabs(1.0 - residual.logden_sub_abs / sub_abs) <= eps;
		ok &= residual.density == d;
		return ok;
	}
}

bool residual_density_xam(void)
{	bool ok    = true;
	double nan = std::numeric_limits<double>::quiet_NaN();
	double pi  = 4.0 * std::atan(1.0);

	using std::sqrt;
	using std::fabs;
	dismod_at::residual_struct<double> residual;
	double wres, smooth, sub_abs, sigma;
	dismod_at::density_enum d;

	double z          = 3.0;
	double y          = 2.5;
	double mu         = 2.0;
	double delta      = 1.5;
	double eta        = nan;

	// -----------------------------------------------------------------------
	bool   difference = true;
	//
	// uniform
	d           = dismod_at::uniform_enum;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	ok         &= residual.wres           == 0.0;
	ok         &= residual.logden_smooth  == 0.0;
	ok         &= residual.logden_sub_abs == 0.0;
	ok         &= residual.density        == d;

	// gaussian
	d           = dismod_at::gaussian_enum;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	wres        = (z - y - mu) / delta;
	smooth      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	sub_abs     = 0.0;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// laplace
	d           = dismod_at::laplace_enum;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	wres        = (z - y - mu) / delta;
	smooth      = - log(delta * sqrt(2.0) );
	sub_abs     =  sqrt(2.0) * wres;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// log-gaussian
	d           = dismod_at::log_gaussian_enum;
	eta         = 0.5;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	sigma       = delta;
	wres        = ( log(z + eta) - log(y + eta) - mu ) / sigma;
	smooth      = - log(sigma * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	sub_abs     = 0.0;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// log-laplace
	d           = dismod_at::log_laplace_enum;
	eta         = 3.0;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	sigma       = delta;
	wres        = ( log(z + eta) - log(y + eta) - mu) / sigma;
	smooth      = - log(sigma * sqrt(2.0) );
	sub_abs     = sqrt(2.0) * wres;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// -----------------------------------------------------------------------
	difference = false;
	//
	// uniform
	d           = dismod_at::uniform_enum;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	ok         &= residual.wres           == 0.0;
	ok         &= residual.logden_smooth  == 0.0;
	ok         &= residual.logden_sub_abs == 0.0;
	ok         &= residual.density        == d;

	// gaussian
	d           = dismod_at::gaussian_enum;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	wres        = (y - mu) / delta;
	smooth      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	sub_abs     = 0.0;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// laplace
	d           = dismod_at::laplace_enum;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	wres        = (y - mu) / delta;
	smooth      = - log(delta * sqrt(2.0) );
	sub_abs     =  sqrt(2.0) * wres;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// log-gaussian
	d           = dismod_at::log_gaussian_enum;
	eta         = 0.5;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	sigma       = log(y + eta + delta) - log(y + eta);
	wres        = ( log(y + eta) - log(mu + eta) ) / sigma;
	smooth      = - log(sigma * sqrt(2.0 * pi) ) - wres * wres / 2.0;
	sub_abs     = 0.0;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	// log-laplace
	d           = dismod_at::log_laplace_enum;
	eta         = 3.0;
	residual    = residual_density(z, y, mu, delta, eta, d, difference);
	sigma       = log(y + eta + delta) - log(y + eta);
	wres        = ( log(y + eta) - log(mu + eta) ) / sigma;
	smooth      = - log(sigma * sqrt(2.0) );
	sub_abs     = sqrt(2.0) * wres;
	ok         &= check(residual, wres, smooth, sub_abs, d, difference);

	return ok;
}
