// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_EXEC_SQL_CMD_HPP
# define DISMOD_AT_EXEC_SQL_CMD_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
   extern void exec_sql_cmd(
      sqlite3*           db,
      const std::string& sql_cmd
   );
   extern std::string exec_sql_cmd(
      sqlite3*           db        ,
      const std::string& sql_cmd   ,
      char               sep
   );
}

# endif
