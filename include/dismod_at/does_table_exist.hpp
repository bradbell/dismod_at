// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_DOES_TABLE_EXIST_HPP
# define DISMOD_AT_DOES_TABLE_EXIST_HPP

# include <sqlite3.h>
# include <string>

namespace dismod_at {
   bool does_table_exist(sqlite3* db,  const std::string table_name);
}

# endif
