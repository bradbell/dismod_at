// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
