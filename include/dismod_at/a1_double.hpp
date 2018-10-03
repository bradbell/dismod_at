// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
