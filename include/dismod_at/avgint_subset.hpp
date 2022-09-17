// SPDX-License-Identifier: AGPL-3.0-or-later
// SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
// SPDX-FileContributor: 2014-22 Bradley M. Bell
// ----------------------------------------------------------------------------
# ifndef DISMOD_AT_AVGINT_SUBSET_HPP
# define DISMOD_AT_AVGINT_SUBSET_HPP

# include <cppad/utility/vector.hpp>
# include "get_data_table.hpp"
# include "get_avgint_table.hpp"
# include "get_covariate_table.hpp"
# include "get_integrand_table.hpp"
# include "child_info.hpp"

namespace dismod_at {
	struct avgint_subset_struct {
		int         original_id;
		int         integrand_id;
		int         node_id;
		int         subgroup_id;
		int         weight_id;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
	};
	extern void avgint_subset(
        const CppAD::vector<integrand_struct>& integrand_table         ,
		const CppAD::vector<avgint_struct>&    avgint_table            ,
		const CppAD::vector<double>&           avgint_cov_value        ,
		const CppAD::vector<covariate_struct>& covariate_table         ,
		const child_info&                      child_info4avgint       ,
		CppAD::vector<avgint_subset_struct>&   avgint_subset_obj       ,
		CppAD::vector<double>&                 avgint_subset_cov_value
	);
}

# endif
