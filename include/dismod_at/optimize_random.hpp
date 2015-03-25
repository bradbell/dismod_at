// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_OPTIMIZE_RANDOM_HPP
# define DISMOD_AT_OPTIMIZE_RANDOM_HPP

# include <cppad/vector.hpp>

namespace dismod_at {
	template <class Logden>
	CppAD::vector<double> optimize_random(
		const CppAD::vector<double>& fixed_vec       ,
		const CppAD::vector<double>& random_in       ,
		Logden&               logden
	);
}

# endif
