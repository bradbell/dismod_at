// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_AVGINT_TABLE_HPP
# define DISMOD_AT_GET_AVGINT_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>
# include "get_covariate_table.hpp"

namespace dismod_at {
	struct avgint_struct {
		int         integrand_id;
		int         node_id;
		int         subgroup_id;
		int         weight_id;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
	};
	extern void get_avgint_table(
		sqlite3* db                                     ,
		size_t                           n_covariate    ,
		double                           age_min        ,
		double                           age_max        ,
		double                           time_min       ,
		double                           time_max       ,
		CppAD::vector<avgint_struct>&    avgint_table   ,
		CppAD::vector<double>&           avgint_cov_value
	);
}

# endif
