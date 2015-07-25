// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CREATE_TABLE_HPP
# define DISMOD_AT_CREATE_TABLE_HPP

# include <sqlite3.h>
# include <string>
# include <cppad/vector.hpp>

namespace dismod_at {
	void create_table(
		sqlite3*                            db             ,
		const std::string&                  table_name     ,
		const CppAD::vector<std::string>&   col_name       ,
		const CppAD::vector<std::string>&   col_type       ,
		const CppAD::vector<bool>&          col_unique     ,
		const CppAD::vector<std::string>&   row_value
	);
}

# endif
