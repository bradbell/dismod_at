// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_NODE_TABLE_HPP
# define DISMOD_AT_GET_NODE_TABLE_HPP

# include <sqlite3.h>
# include <cppad/vector.hpp>

namespace dismod_at {
	struct node_struct {
		std::string node_name;
		int         parent;
	};
	extern CppAD::vector<node_struct> get_node_table(sqlite3*  db);
}

# endif
