// $Id:$
# ifndef DISMOD_AT_MIN_MAX_VECTOR_HPP
# define DISMOD_AT_MIN_MAX_VECTOR_HPP
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
/*
$begin min_max_vector$$

$section Compute Minimum or Maximum Element of a CppAD Vector$$.

$head Syntax$$
$codei%min% = min_vector(%vec%)
%$$
$codei%max% = max_vector(%vec%)
%$$

$head Scalar$$
is the type of element in the vector.

$head vec$$
This arugment has ptotoype
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
	{	Scalar ret = vec[0];
		for(size_t i = 1; i < vec.size(); i++)
			ret = std::min(ret, vec[i]);
		return ret;
	}
	template <class Scalar>
	Scalar max_vector(const CppAD::vector<Scalar>& vec)
	{	Scalar ret = vec[0];
		for(size_t i = 1; i < vec.size(); i++)
			ret = std::max(ret, vec[i]);
		return ret;
	}
}
# endif
