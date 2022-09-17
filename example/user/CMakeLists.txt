# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-22 Bradley M. Bell
# ----------------------------------------------------------------------------
# Build check_example_user
SET(depends "")
#
# BEGIN_SORT_THIS_LINE_PLUS_2
FOREACH(user_case
   age_avg_split
   average_integrand
   bilevel_random
   bnd_mulcov
   cascade
   censor
   change_grid
   compress
   connection_file
   const_random
   const_value
   continue_fit
   csv2db
   data_density
   data_sim
   diff_constraint
   fit_fixed_both
   fit_meas_noise
   fit_random
   fit_sim
   group_mulcov
   hes_fixed_math
   hes_random
   hold_out_1
   hold_out_2
   ill_condition
   jump_at_age
   lasso_covariate
   metropolis
   mulstd
   no_children
   one_function
   plot_curve
   plot_data_fit
   plot_rate_fit
   predict_fit
   predict_mulcov
   re_scale
   residual
   sample_asy
   sample_asy_sim
   sample_sim
   sim_log
   subgroup_mulcov
   sum_residual
   system_command_prc
   trace_init
   trapezoidal
   warm_start
   zsum_child_rate
   zsum_mulcov_meas
   zsum_mulcov_rate
)
# END_SORT_THIS_LINE_MINUS_2
   ADD_CUSTOM_TARGET(
      check_example_user_${user_case}
      bin/user_test.sh example/user/${user_case}.py
      WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
      DEPENDS dismod_at
   )
   SET( depends ${depends} check_example_user_${user_case} )
ENDFOREACH(user_case)
#
ADD_CUSTOM_TARGET( check_example_user DEPENDS ${depends} )
# ---------------------------------------------------------------------------
# Build check_example_user_speed (not included above)
SET(random_seed      0)
SET(n_children       10)
SET(quasi_fixed     "false")
SET(ode_step_size    5)
#
ADD_CUSTOM_TARGET(
   check_example_user_speed
   ${python3_executable} example/user/speed.py
      ${random_seed}
      ${n_children}
      ${quasi_fixed}
      ${ode_step_size}
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
# Build check_example_user_diabetes (not included above)
ADD_CUSTOM_TARGET(
   check_example_user_diabetes
   ${python3_executable} example/user/diabetes.py
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
