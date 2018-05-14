// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-17 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
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
