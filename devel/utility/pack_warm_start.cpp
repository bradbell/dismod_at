// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin pack_warm_start dev}
Packing and Unpacking Warm Start Information in a Vector
########################################################

Syntax
******

| *vec* = ``pack_warm_start`` ( *warm_start* )
| *warm_start* = ``unpack_warm_start`` ( *vec* )

Prototype
*********
{xrst_literal
   // BEGIN_PACK
   // END_PACK
}
{xrst_literal
   // BEGIN_UNPACK
   // END_UNPACK
}

vec
***
This is a single vector that contains a copy of the
warm start information.

warm_start
**********
This is a structure, holding the warm start information
in the form expected by ``cppad_mixed`` .

{xrst_end pack_warm_start}
*/
# include <dismod_at/pack_warm_start.hpp>

namespace {
   struct check_x_info_struct {
      double x; double z_L; double z_U; double scale_x;
   };
   struct check_g_info_struct {
      double lambda; double scale_g;
   };
   struct check_warm_start_struct {
      double mu;
      double scale_f;
      CppAD::vector<check_x_info_struct> x_info;
      CppAD::vector<check_g_info_struct> g_info;
   };
   typedef CppAD::mixed::warm_start_struct warm_start_struct;
   static_assert(
      sizeof(warm_start_struct) == sizeof(check_warm_start_struct) ,
      "pack_warm_start.cpp is out of date with current version of cppad_mixed"
   );
}

namespace dismod_at {

// BEGIN_PACK
CppAD::vector<double> pack_warm_start(
   const CppAD::mixed::warm_start_struct& warm_start )
// END_PACK
{  size_t n    = warm_start.x_info.size();
   size_t m    = warm_start.g_info.size();
   size_t size = 4 + 4 * n + 2 * m;
   CppAD::vector<double> vec(size);
   size_t index = 0;
   vec[index++]   = double(n);
   vec[index++]   = double(m);
   vec[index++]   = warm_start.mu;
   vec[index++]   = warm_start.scale_f;
   for(size_t j = 0; j < n; ++j)
   {  vec[index++] = warm_start.x_info[j].x;
      vec[index++] = warm_start.x_info[j].z_L;
      vec[index++] = warm_start.x_info[j].z_U;
      vec[index++] = warm_start.x_info[j].scale_x;
   }
   for(size_t i = 0; i < m; ++i)
   {  vec[index++] = warm_start.g_info[i].lambda;
      vec[index++] = warm_start.g_info[i].scale_g;
   }
   assert( index == vec.size() );
   //
   // check conversion from size_t to double
   assert( size_t(vec[0]) == n );
   assert( size_t(vec[1]) == m );
   return vec;
}
// BEGIN_UNPACK
CppAD::mixed::warm_start_struct unpack_warm_start(
   const CppAD::vector<double>& vec )
// END_UNPACK
{  CppAD::mixed::warm_start_struct warm_start;
   size_t index = 0;
   //
   size_t n = size_t( vec[index++] );
   warm_start.x_info.resize(n);
   //
   size_t m = size_t( vec[index++] );
   warm_start.g_info.resize(m);
   //
   warm_start.mu      = vec[index++];
   //
   warm_start.scale_f = vec[index++];
   //
   for(size_t j = 0; j < n; ++j)
   {  warm_start.x_info[j].x       = vec[index++];
      warm_start.x_info[j].z_L     = vec[index++];
      warm_start.x_info[j].z_U     = vec[index++];
      warm_start.x_info[j].scale_x = vec[index++];
   }
   for(size_t i = 0; i < m; ++i)
   {  warm_start.g_info[i].lambda  = vec[index++];
      warm_start.g_info[i].scale_g = vec[index++];
   }
   assert( index == vec.size() );
   return warm_start;
}

} // END_DISMOD_AT_NAMESPACE
