# $Id$
#  --------------------------------------------------------------------------
# dismod_at: Estimating Disease Rates as Functions of Age and Time
#           Copyright (C) 2014-20 University of Washington
#              (Bradley M. Bell bradbell@uw.edu)
#
# This program is distributed under the terms of the
#	     GNU Affero General Public License version 3.0 or later
# see http://www.gnu.org/licenses/agpl.txt
# ---------------------------------------------------------------------------
# devel
ADD_LIBRARY(devel EXCLUDE_FROM_ALL
	cmd/depend_command.cpp
	cmd/fit_command.cpp
	cmd/init_command.cpp
	cmd/old2new_command.cpp
	cmd/predict_command.cpp
	cmd/sample_command.cpp
	cmd/set_command.cpp
	cmd/simulate_command.cpp

	model/data_model.cpp
	model/fit_model.cpp
	model/prior_model.cpp
	model/depend.cpp
	model/adj_integrand.cpp
	model/avg_integrand.cpp
	model/avg_noise_effect.cpp
	model/ran_con_rcv.cpp

	utility/avgint_subset.cpp
	utility/bilinear_interp.cpp
	utility/censor_var_limit.cpp
	utility/child_info.cpp
	utility/cohort_ode.cpp
	utility/data_subset.cpp
	utility/eigen_ode2.cpp
	utility/error_exit.cpp
	utility/fixed_effect.cpp
	utility/get_prior_mean.cpp
	utility/random_effect.cpp
	utility/n_random_const.cpp
	utility/residual_density.cpp
	utility/sim_random.cpp
	utility/grid2line.cpp
	utility/split_space.cpp
	utility/pack_info.cpp
	utility/pack_prior.cpp
	utility/time_line_vec.cpp
	utility/age_avg_grid.cpp
	utility/remove_const.cpp
	utility/get_var_limits.cpp

	table/check_child_prior.cpp
	table/check_child_nslist.cpp
	table/check_pini_n_age.cpp
	table/check_rate_limit.cpp
	table/check_table_id.cpp
	table/check_zero_sum.cpp
	table/create_table.cpp
	table/exec_sql_cmd.cpp
	table/get_age_table.cpp
	table/get_avgint_table.cpp
	table/get_option_table.cpp
	table/get_column_max.cpp
	table/get_covariate_table.cpp
	table/get_data_table.cpp
	table/get_density_table.cpp
	table/get_db_input.cpp
	table/get_integrand_table.cpp
	table/get_prior_table.cpp
	table/get_prior_sim_table.cpp
	table/get_mulcov_table.cpp
	table/get_node_table.cpp
	table/get_rate_table.cpp
	table/get_sample_table.cpp
	table/get_data_sim_table.cpp
	table/get_smooth_grid.cpp
	table/get_nslist_pair.cpp
	table/get_nslist_table.cpp
	table/get_smooth_table.cpp
	table/get_table_column.cpp
	table/get_time_table.cpp
	table/get_weight_grid.cpp
	table/get_weight_table.cpp
	table/get_subgroup_table.cpp
	table/log_message.cpp
	table/open_connection.cpp
	table/put_table_row.cpp
	table/smooth_info.cpp
	table/weight_info.cpp
)
# ---------------------------------------------------------------------------
# devel
SET_TARGET_PROPERTIES(devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
#
ADD_EXECUTABLE(dismod_at dismod_at.cpp )
SET_TARGET_PROPERTIES(dismod_at PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}" )
ADD_DEPENDENCIES(dismod_at devel )
TARGET_LINK_LIBRARIES(dismod_at
	devel
	${cppad_mixed_library_path_list}
	${suitesparse_library_path_list}
	${ipopt_library_path_list}
	${gsl_library_path_list}
	${sqlite3_library_path_list}
	${system_specific_library_list}
)
# ---------------------------------------------------------------------------
# install
INSTALL(
	TARGETS dismod_at
	DESTINATION ${dismod_at_prefix}/bin
)
