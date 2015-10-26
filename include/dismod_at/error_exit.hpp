// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_ERROR_EXIT_HPP
# define DISMOD_AT_ERROR_EXIT_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
	extern void error_exit(
		sqlite3*           db           ,
		const std::string& message
	);
	extern void error_exit(
		sqlite3*           db           ,
		const std::string& message      ,
		const std::string& table_name
	);
	extern void error_exit(
		sqlite3*           db           ,
		const std::string& message      ,
		const std::string& table_name   ,
		const size_t&      row_id
	);
}

# endif
