// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_SIM_RANDOM_HPP
# define DISMOD_AT_SIM_RANDOM_HPP

# include <dismod_at/get_density_table.hpp>

namespace dismod_at {
	double sim_random(
		density_enum density,
		double       mu,
		double       delta,
		double       eta,
		double       nu
	);
}

# endif
