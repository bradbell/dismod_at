// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_RESIDUAL_DENSITY_HPP
# define DISMOD_AT_RESIDUAL_DENSITY_HPP

# include <utility>
# include "get_density_table.hpp"

namespace dismod_at {

	template <class Float>
	struct residual_struct {
		Float wres;
		Float logden_smooth;
		Float logden_sub_abs;
	};

	template <class Float>
	residual_struct<Float> residual_density(
		density_enum density ,
		const Float& z       ,
		const Float& mu      ,
		const Float& delta   ,
		const Float& eta
	);
}

# endif
