// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_A1_DOUBLE_HPP
# define DISMOD_AT_A1_DOUBLE_HPP
# include <cppad/cppad.hpp>

/*
$begin a1_double$$
$spell
	CppAD
	namespace
	dismod
	typedef
	hpp
$$

$section Declare a1_double Type$$

$head Syntax$$
$codei%# include <dismod_at/a1_double.hpp>%$$

$head Source Code$$
$srccode%cpp% */
namespace dismod_at {
	typedef CppAD::AD<double>  a1_double;
}
/* %$$
$end
*/

# endif
