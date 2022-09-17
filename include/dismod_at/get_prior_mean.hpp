// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_GET_PRIOR_MEAN_HPP
# define DISMOD_AT_GET_PRIOR_MEAN_HPP

# include "get_prior_table.hpp"
# include "pack_prior.hpp"

namespace dismod_at {
CppAD::vector<double> get_prior_mean(
	const CppAD::vector<dismod_at::prior_struct>& prior_table ,
	const dismod_at::pack_prior&                  var2prior
);
}

# endif
