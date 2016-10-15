// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-16 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_RANDOM_EFFECT_HPP
# define DISMOD_AT_RANDOM_EFFECT_HPP

# include <cppad/cppad.hpp>
# include "pack_info.hpp"

namespace dismod_at {
	extern size_t number_random(const pack_info& pack_object);
	//
	template <class Float>
	extern void unpack_random(
		const pack_info&            pack_object  ,
		const CppAD::vector<Float>& pack_vec     ,
		CppAD::vector<Float>&       random_vec
	);
	//
	template <class Float>
	extern void pack_random(
		const pack_info&            pack_object  ,
		CppAD::vector<Float>&       pack_vec     ,
		const CppAD::vector<Float>& random_vec
	);
}

# endif
