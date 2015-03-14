// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_CHECK_TABLE_ID_HPP
# define DISMOD_AT_CHECK_TABLE_ID_HPP

# include <string>
# include <sqlite3.h>

namespace dismod_at {
	extern size_t check_table_id(sqlite3* db, const std::string& table_name);
}

# endif
