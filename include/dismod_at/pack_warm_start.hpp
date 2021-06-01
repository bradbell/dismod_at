// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_WARM_START_HPP
# define DISMOD_AT_PACK_WARM_START_HPP

# include <cppad/mixed/warm_start_struct.hpp>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	CppAD::vector<double> pack_warm_start(
		const CppAD::mixed::warm_start_struct& warm_start
	);
	CppAD::mixed::warm_start_struct unpack_warm_start(
		const CppAD::vector<double>& vec
	);
}

# endif
