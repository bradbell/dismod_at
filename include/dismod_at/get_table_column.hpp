// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_TABLE_COLUMN_HPP
# define DISMOD_AT_GET_TABLE_COLUMN_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	extern std::string get_table_column_type(
		sqlite3*                    db                    ,
		const std::string&          table_name            ,
		const std::string&          column_name
	);
	extern void get_table_column(
		sqlite3*                    db                    ,
		const std::string&          table_name            ,
		const std::string&          column_name           ,
		CppAD::vector<std::string>& text_result
	);
	extern void get_table_column(
		sqlite3*                    db                    ,
		const std::string&          table_name            ,
		const std::string&          column_name           ,
		CppAD::vector<int>&         int_result
	);
	extern void get_table_column(
		sqlite3*                    db                    ,
		const std::string&          table_name            ,
		const std::string&          column_name           ,
		CppAD::vector<double>&      double_result
	);
}

# endif
