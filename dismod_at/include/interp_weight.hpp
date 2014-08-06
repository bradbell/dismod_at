// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rate Estimation as Functions of Age and Time
          Copyright (C) 2014-14 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the 
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_INTERP_WEIGHT_HPP
# define DISMOD_AT_INTERP_WEIGHT_HPP

# include <cppad/vector.hpp>
# include "weight_info.hpp"

namespace dismod_at {
	extern double interp_weight(
		double                      age        ,
		double                      time       ,
		const weight_info&          w_info         ,
		const CppAD::vector<double> age_table  ,
		const CppAD::vector<double> time_table ,
		size_t&                     i_wi       ,
		size_t&                     j_wi       
	);
}

# endif
