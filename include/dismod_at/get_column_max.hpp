// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
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
