// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_PUT_TABLE_ROW_HPP
# define DISMOD_AT_PUT_TABLE_ROW_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	extern void put_table_row(
		sqlite3*                           db            ,
		const std::string&                 table_name    ,
		const CppAD::vector<std::string>&  col_name_vec  ,
		const CppAD::vector<std::string>&  row_val_vec   ,
		const size_t&                      primary_key
	);
	extern size_t put_table_row(
		sqlite3*                           db            ,
		const std::string&                 table_name    ,
		const CppAD::vector<std::string>&  col_name_vec  ,
		const CppAD::vector<std::string>&  row_val_vec
	);
}

# endif
