// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_INTEGRAND_TABLE_HPP
# define DISMOD_AT_GET_INTEGRAND_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	enum integrand_enum {
		Sincidence_enum,
		Tincidence_enum,
		remission_enum,
		mtexcess_enum,
		mtother_enum,
		mtwith_enum,
		susceptible_enum,
		withC_enum,
		prevalence_enum,
		mtspecific_enum,
		mtall_enum,
		mtstandard_enum,
		relrisk_enum,
		number_integrand_enum
	};
	extern CppAD::vector<integrand_enum> get_integrand_table(sqlite3*  db);
}

# endif
