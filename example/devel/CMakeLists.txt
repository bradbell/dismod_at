# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-22 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build C++ Examples / Tests
# -----------------------------------------------------------------------------
# BEGIN_SORT_THIS_LINE_PLUS_3
ADD_EXECUTABLE(example_devel EXCLUDE_FROM_ALL
	example_devel.cpp
	model/adj_integrand_xam.cpp
	model/avg_integrand_xam.cpp
	model/avg_no_ode_xam.cpp
	model/avg_noise_effect_xam.cpp
	model/avg_yes_ode_xam.cpp
	model/fit_model_xam.cpp
	model/like_all_xam.cpp
	model/like_one_xam.cpp
	model/prior_fixed_xam.cpp
	model/prior_random_xam.cpp
	table/blob_table_xam.cpp
	table/check_pini_n_age_xam.cpp
	table/create_table_xam.cpp
	table/get_age_table_xam.cpp
	table/get_avgint_table_xam.cpp
	table/get_bnd_mulcov_table_xam.cpp
	table/get_column_max_xam.cpp
	table/get_covariate_table_xam.cpp
	table/get_data_sim_table_xam.cpp
	table/get_data_subset_xam.cpp
	table/get_data_table_xam.cpp
	table/get_density_table_xam.cpp
	table/get_integrand_table_xam.cpp
	table/get_mulcov_table_xam.cpp
	table/get_node_table_xam.cpp
	table/get_nslist_pair_xam.cpp
	table/get_nslist_table_xam.cpp
	table/get_option_table_xam.cpp
	table/get_prior_sim_table_xam.cpp
	table/get_prior_table_xam.cpp
	table/get_rate_table_xam.cpp
	table/get_sample_table_xam.cpp
	table/get_smooth_grid_xam.cpp
	table/get_subgroup_table_xam.cpp
	table/get_table_column_xam.cpp
	table/get_time_table_xam.cpp
	table/get_weight_grid_xam.cpp
	table/put_table_row_xam.cpp
	table/smooth_info_xam.cpp
	table/weight_info_xam.cpp
	utility/age_avg_grid_xam.cpp
	utility/avgint_subset_xam.cpp
	utility/balance_pair_xam.cpp
	utility/bilinear_interp_xam.cpp
	utility/child_data_in_fit_xam.cpp
	utility/child_info_xam.cpp
	utility/cohort_ode_xam.cpp
	utility/eigen_ode2_xam.cpp
	utility/fixed_effect_xam.cpp
	utility/grid2line_xam.cpp
	utility/manage_gsl_rng_xam.cpp
	utility/n_random_const_xam.cpp
	utility/pack_info_xam.cpp
	utility/pack_prior_xam.cpp
	utility/random_effect_xam.cpp
	utility/residual_density_xam.cpp
	utility/sim_random_xam.cpp
	utility/split_space_xam.cpp
	utility/subset_data_xam.cpp
	utility/time_line_vec_xam.cpp
)
# END_SORT_THIS_LINE_MINUS_2
#
SET_TARGET_PROPERTIES(
	example_devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(example_devel
	devel
	${cppad_mixed_LIBRARIES}
	${gsl_LIBRARIES}
	${sqlite3_LIBRARIES}
	${ipopt_LIBRARIES}
	${system_specific_library_list}
)
ADD_CUSTOM_TARGET(check_example_devel example_devel DEPENDS example_devel )
ADD_DEPENDENCIES(check_example_devel devel )
