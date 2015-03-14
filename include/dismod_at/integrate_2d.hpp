// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_INTEGRATE_2D_HPP
# define DISMOD_AT_INTEGRATE_2D_HPP

# include <cppad/vector.hpp>
# include <utility>

namespace dismod_at {
	extern CppAD::vector<double> integrate_2d(
		const std::pair<double,double>& a   ,
		const std::pair<double,double>& t   ,
		const std::pair<double,double>& b   ,
		const std::pair<double,double>& s   ,
		const CppAD::vector<double>&    w
	);
}

# endif
