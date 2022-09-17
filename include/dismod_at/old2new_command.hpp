// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_OLD2NEW_COMMAND_HPP
# define DISMOD_AT_OLD2NEW_COMMAND_HPP

# include <sqlite3.h>
# include <dismod_at/get_db_input.hpp>

namespace dismod_at {
	void old2new_command(sqlite3* db);
}

# endif
