// $Id:$
# ifndef DISMOD_AT_MIN_MAX_VECTOR_HPP
# define DISMOD_AT_MIN_MAX_VECTOR_HPP
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
{xrst_begin min_max_vector dev}

Compute Minimum or Maximum Element of a CppAD Vector
####################################################

Syntax
******

| *min* = ``min_vector`` ( *vec* )
| ``max`` = *max_vector* ( ``vec`` )

Scalar
******
is the type of element in the vector.

vec
***
This argument has prototype

   ``const CppAD:vector<`` *Scalar* >& *vec*

It is the vector we are computing the minimum or maximum
value of.

min
***
This return value has prototype

   *Scalar* *min*

and is the minimum value in the vector.

max
***
This return value has prototype

   *Scalar* *max*

and is the maximum value in the vector.

{xrst_end min_max_vector}
*/
# include <cppad/utility/vector.hpp>
namespace dismod_at {
   template <class Scalar>
   Scalar min_vector(const CppAD::vector<Scalar>& vec)
   {  Scalar ret = vec[0];
      for(size_t i = 1; i < vec.size(); i++)
         ret = std::min(ret, vec[i]);
      return ret;
   }
   template <class Scalar>
   Scalar max_vector(const CppAD::vector<Scalar>& vec)
   {  Scalar ret = vec[0];
      for(size_t i = 1; i < vec.size(); i++)
         ret = std::max(ret, vec[i]);
      return ret;
   }
}
# endif
