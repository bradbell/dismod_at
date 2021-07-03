// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_VAR_LIMITS_HPP
# define DISMOD_AT_GET_VAR_LIMITS_HPP

# include <cppad/utility/vector.hpp>
# include "pack_info.hpp"
# include "pack_prior.hpp"
# include "get_prior_table.hpp"

namespace dismod_at {
	void get_var_limits(
		CppAD::vector<double>&               lower_limit  ,
		CppAD::vector<double>&               upper_limit  ,
	    const pack_prior&                    var2prior    ,
		const CppAD::vector<prior_struct>&   prior_table
	);
}

# endif
