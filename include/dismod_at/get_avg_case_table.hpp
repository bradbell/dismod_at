// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_AVG_CASE_TABLE_HPP
# define DISMOD_AT_GET_AVG_CASE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>
# include "get_covariate_table.hpp"

namespace dismod_at {
	struct avg_case_struct {
		int         integrand_id;
		int         node_id;
		int         weight_id;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
	};
	extern void get_avg_case_table(
		sqlite3* db                                     ,
		size_t                           n_covariate    ,
		double                           age_min        ,
		double                           age_max        ,
		double                           time_min       ,
		double                           time_max       ,
		CppAD::vector<avg_case_struct>&  avg_case_table ,
		CppAD::vector<double>&           avg_cov_value
	);
}

# endif
