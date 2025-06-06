# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-25 Bradley M. Bell
# ----------------------------------------------------------------------------
# devel
# BEGIN_SORT_THIS_LINE_PLUS_2
ADD_LIBRARY(devel EXCLUDE_FROM_ALL
   cmd/bnd_mulcov_command.cpp
   cmd/data_density_command.cpp
   cmd/depend_command.cpp
   cmd/fit_command.cpp
   cmd/hold_out_command.cpp
   cmd/init_command.cpp
   cmd/old2new_command.cpp
   cmd/predict_command.cpp
   cmd/sample_command.cpp
   cmd/set_command.cpp
   cmd/simulate_command.cpp
   model/adj_integrand.cpp
   model/avg_integrand.cpp
   model/avg_noise_effect.cpp
   model/data_model.cpp
   model/depend.cpp
   model/fit_model.cpp
   model/prior_model.cpp
   model/ran_con_rcv.cpp
   table/blob_table.cpp
   table/check_child_nslist.cpp
   table/check_child_prior.cpp
   table/check_pini_n_age.cpp
   table/check_rate_eff_cov.cpp
   table/check_rate_limit.cpp
   table/check_table_id.cpp
   table/check_zero_sum.cpp
   table/create_table.cpp
   table/does_table_exist.cpp
   table/exec_sql_cmd.cpp
   table/get_age_table.cpp
   table/get_avgint_table.cpp
   table/get_bnd_mulcov_table.cpp
   table/get_column_max.cpp
   table/get_covariate_table.cpp
   table/get_data_sim_table.cpp
   table/get_data_subset.cpp
   table/get_data_table.cpp
   table/get_db_input.cpp
   table/get_density_table.cpp
   table/get_integrand_table.cpp
   table/get_mulcov_table.cpp
   table/get_node_table.cpp
   table/get_nslist_pair.cpp
   table/get_nslist_table.cpp
   table/get_option_table.cpp
   table/get_prior_sim_table.cpp
   table/get_prior_table.cpp
   table/get_rate_eff_cov_table.cpp
   table/get_rate_table.cpp
   table/get_sample_table.cpp
   table/get_smooth_grid.cpp
   table/get_smooth_table.cpp
   table/get_subgroup_table.cpp
   table/get_table_column.cpp
   table/get_time_table.cpp
   table/get_weight_grid.cpp
   table/get_weight_table.cpp
   table/is_column_in_table.cpp
   table/log_message.cpp
   table/open_connection.cpp
   table/put_table_row.cpp
   table/smooth_info.cpp
   table/weight_info.cpp
   utility/age_avg_grid.cpp
   utility/avgint_subset.cpp
   utility/bilinear_interp.cpp
   utility/censor_var_limit.cpp
   utility/child_data_in_fit.cpp
   utility/child_info.cpp
   utility/cohort_ode.cpp
   utility/cov2weight_map.cpp
   utility/eigen_ode2.cpp
   utility/error_exit.cpp
   utility/fixed_effect.cpp
   utility/get_prior_mean.cpp
   utility/get_str_map.cpp
   utility/get_var_limits.cpp
   utility/grid2line.cpp
   utility/n_random_const.cpp
   utility/pack_info.cpp
   utility/pack_prior.cpp
   utility/pack_warm_start.cpp
   utility/random_effect.cpp
   utility/remove_const.cpp
   utility/residual_density.cpp
   utility/sim_random.cpp
   utility/split_space.cpp
   utility/subset_data.cpp
   utility/time_line_vec.cpp
   utility/trap_ode2.cpp
)
# END_SORT_THIS_LINE_MINUS_2
# ---------------------------------------------------------------------------
# devel
SET_TARGET_PROPERTIES(devel PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}")
#
ADD_EXECUTABLE(dismod_at dismod_at.cpp )
SET_TARGET_PROPERTIES(dismod_at PROPERTIES COMPILE_FLAGS "${extra_cxx_flags}" )
ADD_DEPENDENCIES(dismod_at devel )
TARGET_LINK_LIBRARIES(dismod_at
   devel
   ${cppad_mixed_LIBRARIES}
   ${cppad_LIBRARIES}
   ${gsl_LIBRARIES}
   ${sqlite3_LIBRARIES}
   ${ipopt_LIBRARIES}
   ${system_specific_library_list}
)
# ---------------------------------------------------------------------------
# install
INSTALL(
   TARGETS dismod_at
   DESTINATION ${dismod_at_prefix}/bin
)
