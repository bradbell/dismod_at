// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_SAMPLE_TABLE_HPP
# define DISMOD_AT_GET_SAMPLE_TABLE_HPP

# include <string>
# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	struct sample_struct {
		int         sample_index;
		int         var_id;
		double      var_value;
	};
	extern CppAD::vector<sample_struct> get_sample_table(sqlite3*  db);
}

# endif
