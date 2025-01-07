# SPDX-License-Identifier: AGPL-3.0-or-later
# SPDX-FileCopyrightText: University of Washington <https://www.washington.edu>
# SPDX-FileContributor: 2014-24 Bradley M. Bell
# ----------------------------------------------------------------------------
# Build check_example_user
SET(depends "")
#
# This list below is result of ls ./*.py in this directory
# and removing covid_19.py speed.py,
# and the .py at end of each entry.
#
# BEGIN_SORT_THIS_LINE_PLUS_2
FOREACH(user_case
   age_avg_split
   average_integrand
   bilevel_random
   binomial
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
   diabetes
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
   other_database
   plot_curve
   plot_data_fit
   plot_rate_fit
   predict_fit
   predict_mulcov
   rate_eff_cov_table
   re_scale
   residual
   sample_asy
   sample_asy_sim
   sample_sim
   shock_cov
   sim_log
   subgroup_mulcov
   sum_residual
   system_command_prc
   trace_init
   trapezoidal
   warm_start
   zero_meas_value
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
# These argument values caused CppAD to crash before versions
# 20240214 and 20240000.3; see heading 02-14 on
# https://cppad.readthedocs.io/latest/24.html#whats-new-24-mm-dd-02-14
SET(random_seed      0)
SET(n_children       10)
SET(quasi_fixed     "false")
SET(ode_step_size    5)
SET(n_data           200)
SET(max_fit          50)
#
ADD_CUSTOM_TARGET(
   check_example_user_speed
   ${python3_executable} example/user/speed.py
      ${random_seed}
      ${n_children}
      ${quasi_fixed}
      ${ode_step_size}
      ${n_data}
      ${max_fit}
   WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
   DEPENDS dismod_at
)
# -----------------------------------------------------------------------------
