// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_INTEGRATE_1D_HPP
# define DISMOD_AT_INTEGRATE_1D_HPP

# include <utility>

namespace dismod_at {
	extern std::pair<double, double> integrate_1d(
		const std::pair<double,double>& q   ,
		const std::pair<double,double>& r   ,
		const std::pair<double,double>& w
	);
}

# endif
