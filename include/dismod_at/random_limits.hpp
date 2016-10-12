// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_RANDOM_LIMITS_HPP
# define DISMOD_AT_RANDOM_LIMITS_HPP

# include <cppad/utility/vector.hpp>
# include "pack_info.hpp"
# include "get_prior_table.hpp"
# include "get_smooth_table.hpp"

namespace dismod_at {
	void random_limits(
		const pack_info&                   pack_object   ,
		const CppAD::vector<prior_struct>& prior_table   ,
		const CppAD::vector<smooth_info>&  s_info_vec    ,
		CppAD::vector<double>&             random_lower  ,
		CppAD::vector<double>&             random_upper
	);
}

# endif
