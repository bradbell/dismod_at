// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CHECK_TABLE_ID_HPP
# define DISMOD_AT_CHECK_TABLE_ID_HPP

# include <string>
# include <sqlite3.h>

namespace dismod_at {
	extern size_t check_table_id(sqlite3* db, const std::string& table_name);
}

# endif
