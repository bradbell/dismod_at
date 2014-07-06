// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_DATA_TABLE_HPP
# define DISMOD_AT_GET_DATA_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct data_struct {
		int         data_id;
		int         integrand_id;
		int         density_id;
		int         node_id;
		int         weight_id;
		double      meas_value;
		double      meas_std;
		double      meas_eta;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
		CppAD::vector<double> x;
	};
	extern CppAD::vector<data_struct> get_data_table(
		sqlite3*  db        , 
		size_t n_covariate
	);
}

# endif
