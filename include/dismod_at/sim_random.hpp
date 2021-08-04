// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
