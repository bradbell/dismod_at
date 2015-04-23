// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
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
	CppAD::vector<size_t> pack_prior(
		const pack_info&                     pack_object  ,
		const CppAD::vector<smooth_info>&    s_info_vec
	);
}

# endif
