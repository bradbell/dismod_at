// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-19 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_OLD2NEW_COMMAND_HPP
# define DISMOD_AT_OLD2NEW_COMMAND_HPP

# include <sqlite3.h>
# include <dismod_at/get_db_input.hpp>

namespace dismod_at {
	void old2new_command(sqlite3* db);
}

# endif
