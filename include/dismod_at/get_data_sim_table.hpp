// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_DATA_SIM_TABLE_HPP
# define DISMOD_AT_GET_DATA_SIM_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct data_sim_struct {
		int         simulate_index;
		int         data_subset_id;
		double      data_sim_value;
		double      data_sim_stdcv;
		double      data_sim_delta;
	};
	extern CppAD::vector<data_sim_struct> get_data_sim_table(sqlite3*  db);
}

# endif
