// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_EXEC_SQL_CMD_HPP
# define DISMOD_AT_EXEC_SQL_CMD_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
	extern void exec_sql_cmd(
		sqlite3*           db, 
		const std::string& sql_cmd
	);
}

# endif
