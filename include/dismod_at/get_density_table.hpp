// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Density Estimation as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_GET_DENSITY_TABLE_HPP
# define DISMOD_AT_GET_DENSITY_TABLE_HPP

# include <sqlite3.h>
# include <cppad/utility/vector.hpp>

namespace dismod_at {
	enum density_enum {
		uniform_enum,
		gaussian_enum,
		laplace_enum,
		students_enum,
		log_gaussian_enum,
		log_laplace_enum,
		log_students_enum,
		number_density_enum
	};
	extern CppAD::vector<density_enum> get_density_table(sqlite3*  db);
	//
	// log_density is a function so that if we add another type of log density
	// all the current detection adapts to include the new density.
	template <class type>
	bool log_density(type density_id)
	{	density_enum density = static_cast<density_enum>(density_id);
		bool result = density == log_gaussian_enum;
		result     |= density == log_laplace_enum;
		result     |= density == log_students_enum;
		return result;
	}
}

# endif
