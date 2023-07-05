// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-23 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_IS_COLUMN_IN_TABLE_HPP
# define DISMOD_AT_IS_COLUMN_IN_TABLE_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {

   bool is_column_in_table(
      sqlite3*           db          ,
      const std::string& table_name  ,
      const std::string& column_name
   );

}

# endif
