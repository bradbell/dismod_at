// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin balance_pair_xam.cpp}

C++ balance_pair: Example and Test
##################################

{xrst_literal
   // BEGIN C++
   // END C++
}

{xrst_end balance_pair_xam.cpp}
*/
// BEGIN C++
# include <dismod_at/balance_pair.hpp>

bool balance_pair_xam(void)
{  bool   ok = true;
   using CppAD::vector;
   typedef std::pair<std::string, int> pair_t;
   //
   // n_repeat
   size_t n_repeat = 4;
   //
   // new_gsl_rng
   CppAD::mixed::new_gsl_rng(0);
   //
   std::string first[] = { "England", "Germany", "Italy" };
   int second[]        = {-1, +1};
   //
   // n_first
   size_t n_first = sizeof( first ) / sizeof( first[0] );
   //
   // pair_vec
   vector<pair_t> pair_vec;
   for(size_t i = 0; i < n_repeat; ++i)
      for(size_t j = 0; j < n_first; ++j)
         for(size_t k = 0; k < 2; ++k)
            pair_vec.push_back( pair_t( first[j], second[1-k] ) );
   //
   // n_sample
   size_t n_sample = 8;
   //
   // i_try
   size_t i_try = 0;
   while( i_try < 2 )
   {  ++i_try;
      //
      // sample_vec
      vector<size_t> sample_vec = dismod_at::balance_pair(
         n_sample, pair_vec, second[0], second[1]
      );
      //
      // ok
      // check conditions on first and second
      // also check that random sampling changes second order
      bool ascend  = true;
      bool descend = true;
      for(size_t i = 0; i < n_sample / 2; ++i)
      {  const pair_t& pair_0 = pair_vec[ sample_vec[2 * i] ];
         const pair_t& pair_1 = pair_vec[ sample_vec[2 * i + 1] ];
         ok &= pair_0.first == pair_1.first;
         ok &= pair_0.second + pair_1.second == 0;
         ascend  &= pair_0.second < pair_1.second;
         descend &= pair_0.second > pair_1.second;
      }
      // ok
      if( ascend || descend )
      {  // give up after two tries
         if( i_try == 2 )
            ok = false;
      }
      else
      {  // skip second run of test
         i_try = 2;
      }
   }
   //
   // free_gsn_rng
   CppAD::mixed::free_gsl_rng();
   //
   return ok;
}
// END C++
