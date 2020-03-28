// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-20 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_INTEGRAND_TABLE_HPP
# define DISMOD_AT_GET_INTEGRAND_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include <dismod_at/get_mulcov_table.hpp>
# include <dismod_at/get_option_table.hpp>

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
		mulcov_enum,
		number_integrand_enum
	};
	struct integrand_struct {
		integrand_enum  integrand;
		double          minimum_meas_cv;
		int             mulcov_id;
	};
	extern CppAD::vector<integrand_struct> get_integrand_table(
		sqlite3*                            db           ,
		const CppAD::vector<mulcov_struct>& mulcov_table ,
		const CppAD::vector<option_struct>& option_table
	);
	extern const char* integrand_enum2name[];
}



# endif
