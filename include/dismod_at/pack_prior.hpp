// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-18 University of Washington
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
	struct var_prior_struct {
		size_t  n_time;
		double  const_value;
		size_t  value_prior_id;
		size_t  dage_prior_id;
		size_t  dtime_prior_id;
	};
	CppAD::vector<var_prior_struct>  pack_var_prior(
		const pack_info&                     pack_object  ,
		const CppAD::vector<smooth_info>&    s_info_vec
	);
	//
	struct diff_prior_struct {
		enum { dage_enum, dtime_enum } direction;
		size_t plus_var_id;
		size_t minus_var_id;
		size_t prior_id;
	};
}

# endif
