// $Id$
/* --------------------------------------------------------------------------
dismod_at: Estimating Disease Rates as Functions of Age and Time
          Copyright (C) 2014-21 University of Washington
             (Bradley M. Bell bradbell@uw.edu)

This program is distributed under the terms of the
	     GNU Affero General Public License version 3.0 or later
see http://www.gnu.org/licenses/agpl.txt
-------------------------------------------------------------------------- */
# ifndef DISMOD_AT_DATA_SUBSET_HPP
# define DISMOD_AT_DATA_SUBSET_HPP

# include <cppad/utility/vector.hpp>
# include "get_data_table.hpp"
# include "get_covariate_table.hpp"
# include "get_density_table.hpp"
# include "get_integrand_table.hpp"
# include "child_info.hpp"

namespace dismod_at {
	struct data_subset_struct {
		// values in avgint_subset_struct
		int         original_id;
		int         integrand_id;
		int         node_id;
		int         subgroup_id;
		int         weight_id;
		double      age_lower;
		double      age_upper;
		double      time_lower;
		double      time_upper;
		// values not in avgint_subset_struct
		int          hold_out;
		density_enum density;
		double       meas_value;
		double       meas_std;
		double       eta;
		double       nu;
		// value that depends data_sim table
		double       data_sim_value;
	};
	extern void data_subset(
		const std::string&                     hold_out_integrand      ,
		const CppAD::vector<integrand_struct>& integrand_table         ,
		const CppAD::vector<density_enum>&     density_table           ,
		const CppAD::vector<data_struct>&      data_table              ,
		const CppAD::vector<double>&           data_cov_value          ,
		const CppAD::vector<covariate_struct>& covariate_table         ,
		const child_info&                      child_object            ,
		CppAD::vector<data_subset_struct>&     data_subset_obj         ,
		CppAD::vector<double>&                 data_subset_cov_value
	);
}

# endif
