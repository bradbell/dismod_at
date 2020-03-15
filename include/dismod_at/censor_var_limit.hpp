// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-20 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CENSOR_VAR_LIMIT_HPP
# define DISMOD_AT_CENSOR_VAR_LIMIT_HPP

# include <sqlite3.h>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/pack_prior.hpp>

namespace dismod_at {
	extern void censor_var_limit(
		CppAD::vector<double>&                    var_out       ,
		const CppAD::vector<double>&              var_in        ,
		const pack_prior&                         var2prior     ,
		const CppAD::vector<prior_struct>&        prior_table
	);
}

# endif
