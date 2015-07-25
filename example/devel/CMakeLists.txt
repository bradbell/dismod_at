# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-15 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# Build C++ Examples / Tests

# Release or deug build
SET(CMAKE_BUILD_TYPE DEBUG)

IF( suitesparse_prefix )
	# no pkg-config for SuiteSparse so explicitly set corresponding values
	SET(suitesparse_LIBRARY_DIRS ${suitesparse_prefix}/lib )
	SET(suitesparse_LIBRARIES
		"cholmod;amd;camd;colamd;ccolamd;suitesparseconfig"
	)
	SET(suitesparse_EXAMPLES "approx_mixed/cholmod_xam.cpp")
ELSE( suitesparse_prefix )
	SET(suitesparse_LIBRARY_DIRS "")
	SET(suitesparse_LIBRARIES    "")
	SET(suitesparse_EXAMPLES     "")
ENDIF( suitesparse_prefix )

LINK_DIRECTORIES(
	${ipopt_LIBRARY_DIRS}
	${suitesparse_LIBRARY_DIRS}
	${dismod_at_BINARY_DIR}/cpp
)

ADD_EXECUTABLE(example_devel EXCLUDE_FROM_ALL
	example_devel.cpp

	${suitesparse_EXAMPLES}

	approx_mixed/public/approx_constraint_xam.cpp
	approx_mixed/public/approx_derived_xam.cpp
	approx_mixed/private/constraint_eval_xam.cpp
	approx_mixed/private/constraint_jac_xam.cpp
	approx_mixed/private/constraint_hes_xam.cpp
	approx_mixed/private/eigen_xam.cpp
	approx_mixed/private/joint_grad_ran_xam.cpp
	approx_mixed/private/laplace_hes_fix_xam.cpp
	approx_mixed/private/joint_hes_ran_xam.cpp
	approx_mixed/ipopt_xam_run.cpp
	approx_mixed/private/laplace_eval_xam.cpp
	approx_mixed/private/laplace_beta_xam.cpp
	approx_mixed/public/optimize_fixed_xam.cpp
	approx_mixed/public/optimize_random_xam.cpp
	approx_mixed/private/prior_eval_xam.cpp
	approx_mixed/private/prior_jac_xam.cpp
	approx_mixed/private/prior_hes_xam.cpp
	approx_mixed/user/no_random_xam.cpp

	model/avg_no_ode_xam.cpp
	model/avg_yes_ode_xam.cpp
	model/fit_model_xam.cpp
	model/like_one_xam.cpp
	model/like_all_xam.cpp
	model/prior_fixed_xam.cpp
	model/prior_random_xam.cpp

	utility/avg_case_subset_xam.cpp
	utility/bilinear_interp_xam.cpp
	utility/child_info_xam.cpp
	utility/data_subset_xam.cpp
	utility/eigen_ode2_xam.cpp
	utility/fixed_effect_xam.cpp
	utility/integrate_1d_xam.cpp
	utility/integrate_2d_xam.cpp
	utility/interp_weight_xam.cpp
	utility/manage_gsl_rng_xam.cpp
	utility/pack_info_xam.cpp
	utility/pack_diff_prior_xam.cpp
	utility/pack_value_prior_xam.cpp
	utility/random_effect_xam.cpp
	utility/residual_density_xam.cpp
	utility/sim_random_xam.cpp
	utility/smooth2ode_xam.cpp
	utility/solve_ode_xam.cpp
	utility/to_string_xam.cpp
	utility/variable_name_xam.cpp

	table/check_pini_n_age_xam.cpp
	table/create_table_xam.cpp
	table/get_age_table_xam.cpp
	table/get_avg_case_table_xam.cpp
	table/get_option_table_xam.cpp
	table/get_column_max_xam.cpp
	table/get_covariate_table_xam.cpp
	table/get_data_table_xam.cpp
	table/get_density_table_xam.cpp
	table/get_integrand_table_xam.cpp
	table/get_prior_table_xam.cpp
	table/get_mulcov_table_xam.cpp
	table/get_node_table_xam.cpp
	table/get_rate_table_xam.cpp
	table/get_sample_table_xam.cpp
	table/get_simulate_table_xam.cpp
	table/get_smooth_grid_xam.cpp
	table/get_table_column_xam.cpp
	table/get_time_table_xam.cpp
	table/get_weight_grid_xam.cpp
	table/smooth_info_xam.cpp
	table/put_table_row_xam.cpp
	table/weight_info_xam.cpp
)

LINK_DIRECTORIES(
	${dismod_at_BINARY_DIR}/cpp
	${ipopt_LIBRARY_DIRS}
)
SET_TARGET_PROPERTIES(
	example_devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}"
)
TARGET_LINK_LIBRARIES(example_devel
	devel_lib
	${ipopt_LIBRARIES}
	${suitesparse_LIBRARIES}
	sqlite3
	gsl
	gslcblas
)
ADD_CUSTOM_TARGET(check_example_devel example_devel DEPENDS example_devel )
ADD_DEPENDENCIES(check_example_devel devel_lib)
