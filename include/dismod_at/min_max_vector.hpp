// $Id:$
# ifndef DISMOD_AT_MIN_MAX_VECTOR_HPP
# define DISMOD_AT_MIN_MAX_VECTOR_HPP
// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
/*
$begin min_max_vector$$
$spell
   CppAD
   vec
   const
$$

$section Compute Minimum or Maximum Element of a CppAD Vector$$.

$head Syntax$$
$codei%min% = min_vector(%vec%)
%$$
$codei%max% = max_vector(%vec%)
%$$

$head Scalar$$
is the type of element in the vector.

$head vec$$
This argument has prototype
$codei%
   const CppAD:vector<%Scalar%>& %vec%
%$$
It is the vector we are computing the minimum or maximum
value of.

$head min$$
This return value has prototype
$codei%
   %Scalar% %min%
%$$
and is the minimum value in the vector.

$head max$$
This return value has prototype
$codei%
   %Scalar% %max%
%$$
and is the maximum value in the vector.

$end
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
