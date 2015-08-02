// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_A3_DOUBLE_HPP
# define DISMOD_AT_A3_DOUBLE_HPP
# include<cppad/cppad.hpp>

/*
$begin a3_double$$
$spell
	CppAD
	namespace
	dismod
	typedef
	hpp
$$

$section a3_double$$

$head Syntax$$
$codei%# include <dismod_at/a3_double.hpp>%$$

$head Purpose$$
Defines the type $code a3_double$$ as five levels of $code AD$$ in
the CppAD package.

$head Source Code$$
$codep */
namespace dismod_at {
	typedef CppAD::AD< CppAD::AD< CppAD::AD<double> > > a3_double;
}
/* $$
$end
*/

# endif
