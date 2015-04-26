// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_RUN_TABLE_HPP
# define DISMOD_AT_GET_RUN_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct fit_struct {
		int    parent_node_id;
		double ode_step_size;
		double tolerance;
	};
	extern CppAD::vector<fit_struct> get_fit_table(sqlite3*  db);
}

# endif
