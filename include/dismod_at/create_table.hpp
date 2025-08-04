// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_CREATE_TABLE_HPP
# define DISMOD_AT_CREATE_TABLE_HPP

# include <sqlite3.h>
# include <string>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
   void create_table(
      sqlite3*                            db             ,
      const std::string&                  table_name     ,
      const CppAD::vector<std::string>&   col_name       ,
      const CppAD::vector<std::string>&   col_type       ,
      const CppAD::vector<bool>&          col_unique     ,
      const CppAD::vector<std::string>&   row_value      ,
      const std::size_t&                  cut_size = 1000
   );
}

# endif
