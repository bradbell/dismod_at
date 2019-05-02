// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHECK_VAR_LIMIT_HPP
# define DISMOD_AT_CHECK_VAR_LIMIT_HPP

# include <string>
# include <sqlite3.h>
# include <dismod_at/get_rate_table.hpp>
# include <dismod_at/get_prior_table.hpp>
# include <dismod_at/pack_prior.hpp>

namespace dismod_at {
	extern void check_var_limit(
		const std::string&                        table_name    ,
		size_t                                    table_offset  ,
		const CppAD::vector<double>&              var_value     ,
		const pack_prior&                         var2prior     ,
		const CppAD::vector<prior_struct>&        prior_table
	);
}

# endif
