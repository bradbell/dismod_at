// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_OPEN_CONNECTION_HPP
# define DISMOD_AT_OPEN_CONNECTION_HPP

# include <sqlite3.h>
# include <string>
namespace dismod_at {
	extern sqlite3* open_connection(
		const std::string& file_name,
		bool               new_file
	);
}

# endif
