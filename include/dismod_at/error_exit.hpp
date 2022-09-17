// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_ERROR_EXIT_HPP
# define DISMOD_AT_ERROR_EXIT_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
	void error_exit(
		sqlite3* db
	);
	void error_exit(
		const std::string& message
	);
	void error_exit(
		const std::string& message      ,
		const std::string& table_name
	);
	void error_exit(
		const std::string& message      ,
		const std::string& table_name   ,
		const size_t&      row_id
	);
}

# endif
