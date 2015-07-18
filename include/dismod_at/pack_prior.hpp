// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_PACK_PRIOR_HPP
# define DISMOD_AT_PACK_PRIOR_HPP
# include <dismod_at/pack_info.hpp>
# include <dismod_at/smooth_info.hpp>

namespace dismod_at {
	struct diff_prior_struct {
		size_t plus_var_id;
		size_t minus_var_id;
		size_t prior_id;
	};
	CppAD::vector<size_t> pack_value_prior(
		const pack_info&                     pack_object  ,
		const CppAD::vector<smooth_info>&    s_info_vec
	);
	CppAD::vector<diff_prior_struct> pack_diff_prior(
		const pack_info&                     pack_object  ,
		const CppAD::vector<smooth_info>&    s_info_vec
	);
}

# endif
