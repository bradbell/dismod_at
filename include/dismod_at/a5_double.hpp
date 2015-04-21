// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_A5_DOUBLE_HPP
# define DISMOD_AT_A5_DOUBLE_HPP
# include<cppad/cppad.hpp>

/*
$begin a5_double$$
$spell
	CppAD
	namespace
	dismod
	typedef
$$

$section a5_double$$

$codep */
namespace dismod_at {
	typedef
		CppAD::AD<
		CppAD::AD<
		CppAD::AD<
		CppAD::AD<
		CppAD::AD<
		double
	> > > > > a5_double;
}
/* $$
$end
*/

# endif
