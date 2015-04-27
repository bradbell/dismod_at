// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_OPEN_CONNECTION_HPP
# define DISMOD_AT_OPEN_CONNECTION_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
	extern sqlite3* open_connection(
		const std::string& file_name,
		bool               new_file
	);
}

# endif
