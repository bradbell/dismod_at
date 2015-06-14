// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_COLUMN_MAX_HPP
# define DISMOD_AT_GET_COLUMN_MAX_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
	extern std::string get_column_max(
		sqlite3*           db          ,
		const std::string& table_name  ,
		const std::string& column_name
	);
}

# endif
