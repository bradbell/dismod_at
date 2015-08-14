// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-15 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_AVGINT_SUBSET_HPP
# define DISMOD_AT_AVGINT_SUBSET_HPP

# include <cppad/vector.hpp>
# include "get_data_table.hpp"
# include "get_avgint_table.hpp"
# include "get_covariate_table.hpp"
# include "child_info.hpp"

namespace dismod_at {
	struct avgint_subset_struct {
		int         original_id;
		int         integrand_id;
		int         node_id;
		int         weight_id;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
	};
	extern void avgint_subset(
		const CppAD::vector<avgint_struct>&    avgint_table            ,
		const CppAD::vector<double>&           avgint_cov_value        ,
		const CppAD::vector<covariate_struct>& covariate_table         ,
		const child_info&                      child_object            ,
		CppAD::vector<avgint_subset_struct>&   avgint_subset_obj       ,
		CppAD::vector<double>&                 avgint_subset_cov_value
	);
}

# endif
