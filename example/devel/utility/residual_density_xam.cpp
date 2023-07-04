// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------

# include <limits>
# include <cmath>
# include <dismod_at/residual_density.hpp>

namespace {
   bool check(
      const dismod_at::residual_struct<double>& residual       ,
      double                                    wres           ,
      double                                    smooth         ,
      double                                    sub_abs        ,
      dismod_at::density_enum                   d_id           ,
      size_t                                    index          )
   {  bool ok    = true;
      double eps = 100. * std::numeric_limits<double>::epsilon();
      ok &= fabs(1.0 - residual.wres / wres) <= eps;
      ok &= fabs(1.0 - residual.logden_smooth / smooth) <= eps;
      if( sub_abs == 0.0 )
         ok &= residual.logden_sub_abs == 0.0;
      else
         ok &= fabs(1.0 - residual.logden_sub_abs / sub_abs) <= eps;
      ok &= residual.density == d_id;
      ok &= residual.index   == index;
      return ok;
   }

   double approx_log_binomial(double n, double k, double p)
   {  double n_fac      = n * log(n) - n + 1.0 ;
      double k_fac      = k * log(k) - k + 1.0 ;
      double n_k_fac    = (n - k) * log(n-k) - (n-k) + 1.0 ;
      double n_choose_k = n_fac - k_fac - n_k_fac;
      double approx     = n_choose_k + log(p) * k + log(1.0-p) * (n-k);
      return approx;
   }
}

bool residual_density_xam(void)
{  bool ok    = true;
   double nan = std::numeric_limits<double>::quiet_NaN();
   double pi  = 4.0 * std::atan(1.0);

   using std::sqrt;
   using std::fabs;
   dismod_at::residual_struct<double> residual;
   double wres, smooth, sub_abs;
   size_t                  index;

   dismod_at::density_enum d_id;
   double z              = 0.3;
   double y              = 0.2;
   double mu             = 0.3;
   double delta          = 1.5;
   double d_eta          = nan;
   double d_nu           = 3.0;
   // must choose sample_size and mu so that binomial makes sense; i.e.,
   // so that 0 < mu < 1 and  0 <= y < 1.
   double d_sample_size  = 2;

   // -----------------------------------------------------------------------
   dismod_at::residual_enum residual_type = dismod_at::difference_prior_enum;
   // uniform
   d_id        = dismod_at::uniform_enum;
   index       = 1;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, index
   );
   ok         &= residual.wres           == 0.0;
   ok         &= residual.logden_smooth  == 0.0;
   ok         &= residual.logden_sub_abs == 0.0;
   ok         &= residual.density        == d_id;
   ok         &= residual.index          == index;

   // gaussian
   d_id        = dismod_at::gaussian_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (z - y - mu) / delta;
   smooth      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // laplace
   d_id        = dismod_at::laplace_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (z - y - mu) / delta;
   smooth      = - log(delta * sqrt(2.0) );
   sub_abs     =  sqrt(2.0) * wres;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // students
   d_id        = dismod_at::students_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (z - y - mu) / delta;
   smooth      = - log(1.0 + wres * wres /(d_nu - 2.0) ) * (d_nu + 1.0) / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // log-gaussian
   d_id        = dismod_at::log_gaussian_enum;
   d_eta       = 0.5;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = ( log(z + d_eta) - log(y + d_eta) - mu ) / delta;
   smooth      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // log-laplace
   d_id        = dismod_at::log_laplace_enum;
   d_eta       = 3.0;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = ( log(z + d_eta) - log(y + d_eta) - mu) / delta;
   smooth      = - log(delta * sqrt(2.0) );
   sub_abs     = sqrt(2.0) * wres;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // log-students
   d_id        = dismod_at::log_students_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = ( log(z + d_eta) - log(y + d_eta) - mu) / delta;
   smooth      = - log(1.0 + wres * wres /(d_nu - 2.0) ) * (d_nu + 1.0) / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // -----------------------------------------------------------------------
   residual_type = dismod_at::real_data_enum;
   z     = std::numeric_limits<double>::quiet_NaN();
   //
   // uniform
   d_id        = dismod_at::uniform_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   ok         &= residual.wres           == 0.0;
   ok         &= residual.logden_smooth  == 0.0;
   ok         &= residual.logden_sub_abs == 0.0;
   ok         &= residual.density        == d_id;
   ok         &= residual.index          == index;

   // binomial
   d_id        = dismod_at::binomial_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (y - mu) / delta;
   smooth      = approx_log_binomial(d_sample_size, y * d_sample_size, mu);
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // gaussian
   d_id        = dismod_at::gaussian_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (y - mu) / delta;
   smooth      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // laplace
   d_id        = dismod_at::laplace_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (y - mu) / delta;
   smooth      = - log(delta * sqrt(2.0) );
   sub_abs     =  sqrt(2.0) * wres;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // students
   d_id        = dismod_at::students_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = (y - mu) / delta;
   smooth      = - log(1.0 + wres * wres /(d_nu - 2.0) ) * (d_nu + 1.0) / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // log-gaussian
   d_id        = dismod_at::log_gaussian_enum;
   d_eta       = 0.5;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, +d_sample_size, +index
   );
   wres        = ( log(y + d_eta) - log(mu + d_eta) ) / delta;
   smooth      = - log(delta * sqrt(2.0 * pi) ) - wres * wres / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // log-laplace
   d_id        = dismod_at::log_laplace_enum;
   d_eta       = 3.0;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = ( log(y + d_eta) - log(mu + d_eta) ) / delta;
   smooth      = - log(delta * sqrt(2.0) );
   sub_abs     = sqrt(2.0) * wres;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   // log-students
   d_id        = dismod_at::log_students_enum;
   residual    = residual_density(
      residual_type, z, y, mu, delta, d_id, d_eta, d_nu, d_sample_size, ++index
   );
   wres        = ( log(y + d_eta) - log(mu + d_eta) ) / delta;
   smooth      = - log(1.0 + wres * wres /(d_nu - 2.0) ) * (d_nu + 1.0) / 2.0;
   sub_abs     = 0.0;
   ok         &= check( residual, wres, smooth, sub_abs, d_id, index );

   return ok;
}
