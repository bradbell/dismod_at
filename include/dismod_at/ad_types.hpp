// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_AD_TYPES_HPP
# define DISMOD_AT_AD_TYPES_HPP
# include<cppad/cppad.hpp>

/*
$begin ad_types$$
$spell
	CppAD
	namespace
	dismod
	typedef
	hpp
$$

$section a2_double$$

$head Syntax$$
$codei%# include <dismod_at/ad_types.hpp>%$$

$head Purpose$$
Defines, in the $code dismod_at$$ namespace,
the CppAD types corresponding to one, two and three levels of AD.

$head Source Code$$
$srccode%cpp% */
namespace dismod_at {
	typedef CppAD::AD<double>    a1_double;
	typedef CppAD::AD<a1_double> a2_double;
	typedef CppAD::AD<a2_double> a3_double;
}
/* %$$
$end
*/

# endif
