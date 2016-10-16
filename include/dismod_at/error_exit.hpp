// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
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
	void error_exit(
		sqlite3* db
	);
	void error_exit(
		const std::string& message
	);
	void error_exit(
		const std::string& message      ,
		const std::string& table_name
	);
	void error_exit(
		const std::string& message      ,
		const std::string& table_name   ,
		const size_t&      row_id
	);
}

# endif
