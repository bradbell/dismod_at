// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_BLOB_TABLE_HPP
# define DISMOD_AT_BLOB_TABLE_HPP

# include <sqlite3.h>
# include <string>

namespace dismod_at {
	void write_blob_table(
		sqlite3*                            db             ,
		const std::string&                  table_name     ,
		const std::string&                  col_name       ,
		const size_t&                       sizeof_data    ,
		const void*                         data
	);
	void read_blob_table(
		sqlite3*                            db             ,
		const std::string&                  table_name     ,
		const std::string&                  col_name       ,
		size_t&                             sizeof_data    ,
		void*                               data
	);
}

# endif
