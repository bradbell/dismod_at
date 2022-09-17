// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin bilinear_interp_xam.cpp$$
$spell
   interp
   xam
$$

$section C++ bilinear_interp: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <limits>
# include <cmath>
# include <dismod_at/bilinear_interp.hpp>

bool bilinear_interp_xam(void)
{
   bool   ok = true;
   using  std::string;
   using  CppAD::vector;

   double eps = std::numeric_limits<double>::epsilon();

   size_t n_x = 4, n_y = 3;
   vector<double> x_grid(n_x), y_grid(n_y), z_grid(n_x * n_y);
   for(size_t i = 0; i < n_x; i++)
      x_grid[i] = 10.0 * double(i);
   for(size_t j = 0; j < n_y; j++)
      y_grid[j] = 1990.0 + 10 * double(j);
   for(size_t i = 0; i < n_x; i++)
   {  for(size_t j = 0; j < n_y; j++)
         z_grid[i * n_y + j] = x_grid[i] * y_grid[j] + 1.0;
   }

   // check case where both x and y are interpolated
   double x = 10.0 * 2.5;
   double y = 1990.0 + 10 * 1.5;
   size_t i = 0;
   size_t j = 0;
   // evaluate the interpolant
   double z = dismod_at::bilinear_interp(
      x, y, x_grid, y_grid, z_grid, i, j
   );
   ok = x_grid[i] <= x && x <= x_grid[i+1];
   ok = y_grid[j] <= y && y <= y_grid[j+1];

   double z_check  = x * y + 1.0;
   ok = std::fabs( 1.0 - z / z_check  ) < 10.0 * eps;

   // check case where only y is interpolated ( x < x_grid[0] )
   x = x_grid[0] - 10.;
   y = 1990.0 + 10 * 1.5;
   z = dismod_at::bilinear_interp(
      x, y, x_grid, y_grid, z_grid, i, j
   );
   z_check  = x_grid[0] * y + 1.0;
   ok       =  i == 0;
   ok       = y_grid[j] <= y && y <= y_grid[j+1];
   ok       = std::fabs( 1.0 - z / z_check  ) < 10.0 * eps;

   // check case where only x is interpolated ( y_grid[n_y-1] < y )
   x = 10.0 * 2.5;
   y = y_grid[n_y-1] + 10.0;
   z = dismod_at::bilinear_interp(
      x, y, x_grid, y_grid, z_grid, i, j
   );
   z_check  = x * y_grid[n_y-1] + 1.0;
   ok       = x_grid[i] <= x && x <= x_grid[i+1];
   ok       = j == (n_y-1);
   ok       = std::fabs( 1.0 - z / z_check  ) < 10.0 * eps;


   // check case where no interpolation is done
   // ( x_grid[n_x-1] < x and y < y_grid[0] )
   x = x_grid[n_x-1] + 10.0;
   y = y_grid[0]     - 10.0;
   z = dismod_at::bilinear_interp(
      x, y, x_grid, y_grid, z_grid, i, j
   );
   z_check  = x_grid[n_x-1] * y_grid[0] + 1.0;
   ok       = i == (n_x-1);
   ok       = j == 0;
   ok       = std::fabs( 1.0 - z / z_check  ) < 10.0 * eps;


   return ok;
}
// END C++
