// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_LOG_MESSAGE_HPP
# define DISMOD_AT_LOG_MESSAGE_HPP

# include <sqlite3.h>
# include <string>
# include <ctime>

namespace dismod_at {
   extern std::time_t log_message(
      sqlite3*           db           ,
      std::ostream*      os           ,
      const std::string& message_type ,
      const std::string& message
   );
   extern std::time_t log_message(
      sqlite3*           db           ,
      std::ostream*      os           ,
      const std::string& message_type ,
      const std::string& message      ,
      const std::string& table_name   ,
      const size_t&      row_id
   );
}

# endif
