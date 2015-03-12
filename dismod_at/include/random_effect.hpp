// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_RANDOM_EFFECT_HPP
# define DISMOD_AT_RANDOM_EFFECT_HPP

# include <cppad/cppad.hpp>
# include <dismod_at/include/pack_var.hpp>

namespace dismod_at {
	extern size_t size_random_effect(const pack_var& pack_info);

	template <class Float>
	extern void get_random_effect(
		const pack_var&             pack_info  ,
		const CppAD::vector<Float>& pack_vec   ,
		CppAD::vector<Float>&       random_vec
	);
}

# endif
