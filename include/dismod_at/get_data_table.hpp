// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_DATA_TABLE_HPP
# define DISMOD_AT_GET_DATA_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>
# include "get_covariate_table.hpp"

namespace dismod_at {
	struct data_struct {
		int         integrand_id;
		int         density_id;
		int         node_id;
		int         weight_id;
		int         hold_out;
		double      meas_value;
		double      meas_std;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
	};
	extern void get_data_table(
		sqlite3* db                                 ,
		size_t                      n_covariate     ,
		double                      age_min         ,
		double                      age_max         ,
		double                      time_min        ,
		double                      time_max        ,
		CppAD::vector<data_struct>& data_table      ,
		CppAD::vector<double>&      data_cov_value
	);
}

# endif
