/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DEPEND_HPP
# define DISMOD_AT_DEPEND_HPP

# include "data_model.hpp"
# include "prior_model.hpp"

namespace dismod_at {
	// data_depend
	CppAD::vector<bool> data_depend(
		const CppAD::vector<double>& pack_vec     ,
		data_model&                  data_object
	);
	// prior_depend
	CppAD::vector<bool> prior_depend(
		const CppAD::vector<double>& pack_vec     ,
		const prior_model&           prior_object
	);
}

# endif
