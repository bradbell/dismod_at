// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin age_avg_grid_xam.cpp$$
$spell
   xam
$$

$section C++ age_avg_grid: Example and Test$$

$srcthisfile%0%// BEGIN C++%// END C++%1%$$

$end
*/
// BEGIN C++
# include <dismod_at/age_avg_grid.hpp>

bool age_avg_grid_xam(void)
{  using CppAD::vector;

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
