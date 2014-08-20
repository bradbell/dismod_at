// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DENSITY_HPP
# define DISMOD_AT_DENSITY_HPP

# include <utility>

namespace dismod_at { 

	template <class Float>
	Float log_gaussian(
		const Float& mu    , 
		const Float& sigma , 
		const Float& z
	);
	template <class Float>
	std::pair<Float,Float> log_gaussian(
		const Float& mu    , 
		const Float& sigma , 
		const Float& z
	);

}

# endif
