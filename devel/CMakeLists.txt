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
SET(CMAKE_BUILD_TYPE DEBUG)
ADD_LIBRARY(devel_lib EXCLUDE_FROM_ALL
	approx_mixed/joint_grad_ran.cpp
	approx_mixed/laplace_hes_fix.cpp
	approx_mixed/joint_hes_ran.cpp
	approx_mixed/initialize.cpp
	approx_mixed/ipopt_fixed.cpp
	approx_mixed/laplace_eval.cpp
	approx_mixed/laplace_beta.cpp
	approx_mixed/optimize_fixed.cpp
	approx_mixed/optimize_random.cpp
	approx_mixed/prior_eval.cpp
	approx_mixed/prior_jac.cpp
	approx_mixed/prior_hes.cpp
	approx_mixed/record_prior.cpp
	approx_mixed/record_grad_ran.cpp
	approx_mixed/record_hes_fix.cpp
	approx_mixed/record_hes_ran.cpp
	approx_mixed/record_joint.cpp
	approx_mixed/record_laplace.cpp

	model/data_model.cpp
	model/fit_model.cpp
	model/prior_model.cpp

	utility/avg_case_subset.cpp
	utility/bilinear_interp.cpp
	utility/child_info.cpp
	utility/data_subset.cpp
	utility/eigen_ode2.cpp
	utility/fixed_effect.cpp
	utility/integrate_1d.cpp
	utility/integrate_2d.cpp
	utility/interp_weight.cpp
	utility/manage_gsl_rng.cpp
	utility/random_effect.cpp
	utility/residual_density.cpp
	utility/sim_random.cpp
	utility/smooth2ode.cpp
	utility/solve_ode.cpp
	utility/pack_info.cpp
	utility/pack_prior.cpp

	table/check_child_prior.cpp
	table/check_pini_n_age.cpp
	table/check_table_id.cpp
	table/exec_sql_cmd.cpp
	table/get_age_table.cpp
	table/get_avg_case_table.cpp
	table/get_argument_table.cpp
	table/get_column_max.cpp
	table/get_covariate_table.cpp
	table/get_data_table.cpp
	table/get_density_table.cpp
	table/get_db_input.cpp
	table/get_integrand_table.cpp
	table/get_prior_table.cpp
	table/get_mulcov_table.cpp
	table/get_node_table.cpp
	table/get_rate_table.cpp
	table/get_sample_table.cpp
	table/get_simulate_table.cpp
	table/get_smooth_grid.cpp
	table/get_smooth_table.cpp
	table/get_table_column.cpp
	table/get_time_table.cpp
	table/get_weight_grid.cpp
	table/get_weight_table.cpp
	table/log_message.cpp
	table/open_connection.cpp
	table/put_table_row.cpp
	table/smooth_info.cpp
	table/table_error_exit.cpp
	table/weight_info.cpp
)
SET_TARGET_PROPERTIES(devel_lib PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
#
LINK_DIRECTORIES( ${ipopt_LIBRARY_DIRS} )
ADD_EXECUTABLE( dismod_at dismod_at.cpp )
SET_TARGET_PROPERTIES( dismod_at PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}" )
ADD_DEPENDENCIES(dismod_at devel_lib)
TARGET_LINK_LIBRARIES(dismod_at
	devel_lib
	${ipopt_LIBRARIES}
	sqlite3
	gsl
	gslcblas
)
INSTALL( TARGETS dismod_at DESTINATION ${dismod_at_prefix}/bin )
