// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_N_RANDOM_CONST_HPP
# define DISMOD_AT_N_RANDOM_CONST_HPP

# include <cppad/cppad.hpp>
# include "pack_info.hpp"
# include "pack_prior.hpp"
# include "smooth_info.hpp"
# include "get_prior_table.hpp"

namespace dismod_at {
	extern size_t number_random_const(
		const pack_info&                     pack_object  ,
		const pack_prior&                    var2prior    ,
		const CppAD::vector<prior_struct>&   prior_table
	);
}

# endif
