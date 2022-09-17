// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_DENSITY_TABLE_HPP
# define DISMOD_AT_GET_DENSITY_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	enum density_enum {
		uniform_enum,
		gaussian_enum,
		cen_gaussian_enum,
		log_gaussian_enum,
		cen_log_gaussian_enum,
		laplace_enum,
		cen_laplace_enum,
		log_laplace_enum,
		cen_log_laplace_enum,
		students_enum,
		log_students_enum,
		number_density_enum
	};
	extern CppAD::vector<density_enum> get_density_table(sqlite3*  db);
	extern bool log_density(density_enum density);
	extern bool nonsmooth_density(density_enum density);
	extern bool censored_density(density_enum density);
	extern const char* density_enum2name[];
}

# endif
