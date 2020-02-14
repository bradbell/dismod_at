// ------------------------------------------------------------ 
// Copyright (C) Bradley M. Bell 1998-2015, All rights reserved 
// ------------------------------------------------------------ 
Keyword = 
[
{ tag: 'dismod_at', title:'dismod_at-20200214: User Documentation and API', other:' purpose links' },
{ tag: 'math_abstract', title:'An Introduction To The Mathematics of Dismod_at', other:'' },
{ tag: 'math_ode', title:'The Dismod_at Ordinary Differential Equation', other:' susceptible s(a t) with condition c(a prevalence p(a incidence iota(a remission rho(a excess mortality chi(a other cause omega(a initial pini(t)' },
{ tag: 'censor_density', title:'The Censored Gaussian and Laplace Densities', other:' references discussion simulation test g(ymudelta) error function g(ymudeltac) difference between means l(ymudelta) indefinite integral l(ymudeltac)' },
{ tag: 'install', title:'Installing dismod_at', other:'' },
{ tag: 'dock_dismod_at.sh', title:'Install and Run dismod_at in a Docker Image', other:' syntax logging output purpose requirements building images version dismod_at.base dismod_at.image removing containers dockerfile errors new build_type database command other arguments debugging determine id start stop' },
{ tag: 'install_unix', title:'Installing dismod_at in Unix', other:' system requirements c++ compiler fortran git cmake pkg-config python3 sqlite3 gsl suitesparse download dismod_at.git paths adding to pkg_config_path ld_library_path pythonpath special run_cmake.sh install_eigen.sh install_ipopt.sh install_cppad.sh install_cppad_mixed.sh setup command build test example' },
{ tag: 'run_cmake.sh', title:'bin/run_cmake.sh: User Configuration Options', other:' verbose_makefile build_type dismod_at_prefix eigen_prefix ipopt_prefix cppad_prefix debug release python3_executable choosing c++ compiler extra_cxx_flags cmake_libdir system_specific_library_list' },
{ tag: 'example_install.sh', title:'An Example Installation', other:' syntax' },
{ tag: 'get_started', title:'Dismod_at Getting Started Examples / Tests', other:' see also database run one get_started.db commands' },
{ tag: 'get_started_db.py', title:'Create get_started Input Tables: Example and Test', other:' syntax discussion reference source code' },
{ tag: 'user', title:'User Examples and Tests', other:' see also run one user_speed.py example.db with explanation more' },
{ tag: 'user_age_avg_split.py', title:'Non-uniform Age Average Grid', other:' purpose variables prior data fit predict ode_step_size source code' },
{ tag: 'user_bilevel_random.py', title:'Example Fitting With Two Levels of Random Effects', other:' node table problem parameters model variables n1 n11 n12 n111 n112 n121 122 subgroup mulcov data procedure both compare truth sample posterior check coverage source code' },
{ tag: 'user_cascade.py', title:'Generating Priors For Next Level Down Node Tree', other:' table problem procedure step 1: create database 2: fit with n1 parent 3: simulate 4: sample posterior 5: predictions n11 6: 7: parameters age time values rate covariates multipliers gamma alpha source code' },
{ tag: 'user_censor.py', title:'Fitting Data That Has Negative Values Censored', other:' purpose problem parameters age time variables rate table prior discussion source code' },
{ tag: 'user_change_grid.py', title:'Remove an Age or Time From a Smoothing', other:' purpose table parent value prior difference fit source code' },
{ tag: 'user_const_random.py', title:'Fitting With Non-Zero Constant Random Effects in Smoothing Grid', other:' source code' },
{ tag: 'user_const_value.py', title:'Constrain Omega Using const_value', other:' node table problem parameters child random effects priors nslist iota parent data random_seed source code' },
{ tag: 'user_continue_fit.py', title:'Continuing a Fit From Where it Left Off', other:' option table source code' },
{ tag: 'user_csv2db.py', title:'csv2db_command: Example and Test', other:' using this discussion rate_true grids data predictions mtall source' },
{ tag: 'user_data_sim.py', title:'Explanation of Simulated Data Table, data_sim', other:' random effects priors iota other rates covariate multiplier subset meas_noise_effect scaling gamma source code' },
{ tag: 'user_diabetes.py', title:'An Example / Speed Test Fitting Simulated Diabetes Data', other:' running this omega rho covariates table multipliers truth parent rates child effects predict problem parameters mulcov_dict node_list integrand_list parent_age_grid child_age_grid parent_time_grid child_time_grid ode_step_size meas_cv meas_repeat fit_with_noise_in_data random_seed quasi_fixed tolerance_fixed derivative_test_fixed truth2start accept_rel_err source code' },
{ tag: 'user_diff_constraint.py', title:'Fitting with Constraints on Differences in Age and Time', other:' source code' },
{ tag: 'user_fit_fixed_both.py', title:'Fit Fixed First Then Both', other:' purpose discussion source code' },
{ tag: 'user_fit_meas_noise.py', title:'Group Measurement Noise Covariate Multipliers, Gamma', other:' purpose random effects iota other rates subgroup table data meas_noise_effect scaling source code' },
{ tag: 'user_fit_random.py', title:'Fitting Just Random Effects', other:' purpose discussion source code' },
{ tag: 'user_fit_sim.py', title:'Fitting Simulated Data Example', other:' parent iota child other rates covariate multiplier starting point scaling priors source code' },
{ tag: 'user_group_mulcov.py', title:'Using Group Covariate Multipliers', other:' see also purpose true value variables integrand node tables subgroup covariates simulated data rate source code' },
{ tag: 'user_ill_condition.py', title:'An Ill Conditioned Example Where Re-Scaling is Helpful', other:' notation objective derivative ill-conditioning source code' },
{ tag: 'user_jump_at_age.py', title:'Jump in Rate Value at a Known Age', other:' purpose model variables truth simulated data ages table time source code' },
{ tag: 'user_lasso_covariate.py', title:'Using Lasso on Covariate Multiplier', other:' see also purpose problem parameters age time values variables table mulcov rate data prior iota alpha source code' },
{ tag: 'user_mulstd.py', title:'Estimating Smoothing Standard Deviation Multiplies', other:' purpose problem parameters age time values variables parent children data table rate prior fitting source code' },
{ tag: 'user_no_children.py', title:'Case with no Children; i.e., no Random Effects', other:' source code' },
{ tag: 'user_posterior.py', title:'Sampling From Then Posterior Distribution', other:' purpose discussion variance source code' },
{ tag: 'user_predict_fit.py', title:'Predict Average Integrand Using Results of a Fit', other:' purpose note table problem parameters time values rate variables data avgint source code' },
{ tag: 'user_predict_mulcov.py', title:'Predict Covariate Multiplier Values', other:' purpose problem parameters age time rate table variables integrand data avgint source code' },
{ tag: 'user_re_scale.py', title:'Case Where Re-Scaling is Useful', other:' source code' },
{ tag: 'user_sample_asy.py', title:'Sample from Asymptotic Distribution for Model Variables', other:' purpose notation likelihood source code' },
{ tag: 'user_sample_sim.py', title:'Sample Posterior Using Simulated Data', other:' purpose node table model variables iota x parameters delta values likelihood number_sample number_metropolis truth random_seed source code' },
{ tag: 'user_speed.py', title:'A Simulate Data Speed Test', other:' syntax python3 random_seed n_children quasi_fixed ode_step_size source code' },
{ tag: 'user_students.py', title:'Using Student\'s-t to Fit With Remove Outliers Present', other:' purpose problem parameters age time values variables data table rate prior fitting source code' },
{ tag: 'user_zsum_child_rate.py', title:'Constrain Sum of Child Rate Effect to Zero', other:' see also purpose problem parameters data simulation nodes model variables source code' },
{ tag: 'user_zsum_mulcov_meas.py', title:'Constrain Sum of Subgroup Measurement Covariate Multipliers to Zero', other:' see also purpose problem parameters data simulation nodes model variables source code' },
{ tag: 'user_zsum_mulcov_rate.py', title:'Constrain Sum of Subgroup Rate Covariate Multipliers to Zero', other:' see also purpose problem parameters data simulation nodes model variables source code' },
{ tag: 'database', title:'The Dismod_at Database Tables', other:' comment columns primary key name foreign parent text can be null not empty integer real nan bounds that type names' },
{ tag: 'input', title:'Tables That Are Only Used as Inputs', other:'' },
{ tag: 'age_table', title:'The Age Table', other:' age_id average grid example' },
{ tag: 'age_table.py', title:'age_table: Example and Test', other:'' },
{ tag: 'avgint_table', title:'The avgint Table: Defines Average Integrand Cases', other:' see also purpose usage parent rates child avgint_id integrand_id node_id subgroup_id weight_id null age_lower age_upper time_lower time_upper covariates example' },
{ tag: 'avgint_table.py', title:'avgint_table: Example and Test', other:'' },
{ tag: 'covariate_table', title:'The Covariate Table', other:' covariate_id covariate_name reference max_difference data subset avgint example' },
{ tag: 'covariate_table.py', title:'covariate_table: Example and Test', other:'' },
{ tag: 'data_table', title:'The Data Table', other:' data_id data_name integrand_id density_id nonsmooth node_id parent child subgroup_id weight_id null hold_out meas_value meas_std adjusted standard deviation delta linear scaling log eta age_lower age_upper time_lower time_upper covariates example' },
{ tag: 'data_table.py', title:'data_table: Example and Test', other:'' },
{ tag: 'density_table', title:'The Density Table', other:' density_id density_name uniform gaussian cen_gaussian log_gaussian cen_log_gaussian laplace cen_laplace log_laplace cen_log_laplace students log_students notation linear censored scaled nonsmooth example' },
{ tag: 'density_table.py', title:'density_table: Example and Test', other:'' },
{ tag: 'integrand_table', title:'The Integrand Table', other:' integrand_id minimum_meas_cv integrand_name ode names example' },
{ tag: 'integrand_table.py', title:'integrand_table: Example and Test', other:'' },
{ tag: 'mulcov_table', title:'The mulcov Table: Covariate Multipliers', other:' mulcov_id mulcov_type rate_value meas_value meas_noise rate_id integrand_id covariate_id group_id group_smooth_id pini subgroup_smooth_id example' },
{ tag: 'mulcov_table.py', title:'mulcov_table: Example and Test', other:'' },
{ tag: 'node_table', title:'The Node Table', other:' node_id node_name parent children descendant example' },
{ tag: 'node_table.py', title:'node_table: Example and Test', other:'' },
{ tag: 'nslist_pair_table', title:'Lists of Node Smoothing Pairs', other:' nslist_pair_id nslist_id node_id smooth_id example' },
{ tag: 'nslist_pair_table.py', title:'nslist_pair_table: Example and Test', other:'' },
{ tag: 'nslist_table', title:'The Node Smoothing List Table', other:' nslist_id nslist_name example' },
{ tag: 'nslist_table.py', title:'nslist_table: Example and Test', other:'' },
{ tag: 'option_table', title:'The Option Table', other:' conventions changing values option_id option_name option_value parent_node_id default parent_node_name children data subset avgint meas_noise_effect zero_sum_child_rate zero_sum_mulcov_group data_extra_columns avgint_extra_columns warn_on_stderr minimum_meas_cv ode_step_size age_avg_split average grid random_seed rate_case iota_zero_rho_zero iota_pos_rho_zero iota_zero_rho_pos iota_pos_rho_pos fixed derivative_test max_num_iter print_level accept_after_max_steps tolerance only quasi_fixed bound_frac_fixed limited_memory_max_history_fixed method_random bound_random example' },
{ tag: 'option_table.py', title:'option_table: Example and Test', other:'' },
{ tag: 'prior_table', title:'The Prior Table', other:' purpose prior_id prior_name density_id lower upper mean uniform std linear scaling log discussion eta null fixed effects example' },
{ tag: 'prior_table.py', title:'prior_table: Example and Test', other:'' },
{ tag: 'rate_table', title:'The Rate Table', other:' rate_id rate_name pini iota rho chi omega parent_smooth_id null child_smooth_id random effects hessian child_nslist_id value priors constant not difference example' },
{ tag: 'rate_table.py', title:'rate_table: Example and Test', other:'' },
{ tag: 'smooth_grid_table', title:'The Smooth Grid Table', other:' purpose smooth_grid_id smooth_id null age_id time_id value_prior_id dage_prior_id maximum dtime_prior_id const_value rectangular example' },
{ tag: 'smooth_grid_table.py', title:'smooth_grid_table: Example and Test', other:'' },
{ tag: 'smooth_table', title:'The Smoothing Table', other:' purpose empty smooth_id smooth_name n_age n_time mulstd_value_prior_id null mulstd_dage_prior_id mulstd_dtime_prior_id removing example' },
{ tag: 'subgroup_table', title:'The Subgroup Table', other:' purpose subgroup_id subgroup_name discussion spaces order example' },
{ tag: 'time_table', title:'The Time Table', other:' time_id ode grid example' },
{ tag: 'time_table.py', title:'time_table: Example and Test', other:'' },
{ tag: 'weight_grid_table', title:'The Weight Grid Table', other:' weight_grid_id weight_id age_id time_id rectangular example' },
{ tag: 'weight_grid_table.py', title:'weight_grid_table: Example and Test', other:'' },
{ tag: 'weight_table', title:'The Weight Table', other:' weight_id weight_name n_age n_time example' },
{ tag: 'data_flow', title:'The Dismod_at Data Flow', other:' output tables input command' },
{ tag: 'log_table', title:'The Log Table', other:' log_id message_type command random_seed error warning table_name row_id unix_time' },
{ tag: 'var_table', title:'Identifying Model Variables', other:' var_id var_type mulstd_value mulstd_dage mulstd_dtime rate mulcov_rate_value mulcov_meas_value mulcov_meas_noise smooth_id age_id time_id node_id rate_id integrand_id covariate_id mulcov_id group_id otherwise subgroup_id example' },
{ tag: 'data_subset_table', title:'The Data Subset Table', other:' data_subset_id data_id discussion node covariates example' },
{ tag: 'depend_var_table', title:'Which Variables The Model Depends On', other:' purpose depend_var_id data_depend prior_depend example' },
{ tag: 'start_var_table', title:'Starting Values Used During a Fit', other:' discussion start_var_id start_var_value example' },
{ tag: 'scale_var_table', title:'Scaling Fixed Effects Objective and Constraints', other:' discussion scale_var_id scale_var_value example' },
{ tag: 'age_avg_table', title:'The Age Average Grid', other:' creation age_avg_id' },
{ tag: 'fit_var_table', title:'The Optimization Results for Variables', other:' lagrange multipliers fit_var_id fit_var_value residual_value residual_dage residual_dtime lagrange_value lagrange_dage lagrange_dtime example' },
{ tag: 'fit_data_subset_table', title:'The Model and Weighted Residuals Corresponding to a Fit', other:' fit_data_subset_id avg_integrand weighted_residual example' },
{ tag: 'truth_var_table', title:'True Values Used During Simulations', other:' discussion truth_var_id truth_var_value example' },
{ tag: 'prior_sim_table', title:'Simulated Variation in Prior', other:' discussion prior_sim_id simulate_index var_id prior_sim_value zero_sum_child_rate zero_sum_mulcov_group prior_sim_dage prior_sim_dtime censoring example' },
{ tag: 'data_sim_table', title:'Simulated Measurements and Adjusted Standard Deviations', other:' data_sim_id simulate_index data_subset_id data_sim_value data_sim_stdcv data_sim_delta method data_id y eta sigma z linear log scaled example' },
{ tag: 'sample_table', title:'The Sample Table: Samples of Variable Values', other:' simulated data user\'s choice sample_id sample_index var_id var_value example' },
{ tag: 'predict_table', title:'The Predict Table: Average Integrand Predictions', other:' see also purpose avgint subset node covariates predict_id sample_index avgint_id avg_integrand example' },
{ tag: 'model', title:'The Age-Time Dismod Model', other:'' },
{ tag: 'model_variables', title:'The Model Variables', other:' introduction prior standard deviation multipliers functions age time children fixed effects theta smoothing lambda parent rates group covariate random subgroup variation initial prevalence other cases' },
{ tag: 'avg_integrand', title:'Model for the Average Integrand', other:' ordinary differential equation data avgint table notation a_i b_i c_i d_i s_i covariate difference x_ij w_i n_i rate functions parent q_k child effect u_ik j_k group multiplier alpha_jk subgroup delta adjusted r_ik pini p_i0(t) iota_i(at) rho_i(at) chi_i(at) omega_i(at) s_i(at) c_i(at) sincidence remission mtexcess mtother mtwith susceptible withc prevalence tincidence mtspecific mtall mtstandard relrisk mulcov measurement value covariates k_i beta_j weight integral wbar_i' },
{ tag: 'data_like', title:'Data Likelihood and Weighted Residuals', other:' table notation y_i a_i b_i s_i t_i d_i i_i x_ij w_i minimum cv standard deviation delta_i measurement noise covariates k_i gamma_j effects theta average integrand adjusted scaling add variance add_std_scale_all add_std_scale_log add_var_scale_all add_var_scale_log' },
{ tag: 'variable_prior', title:'Prior for the Model Variables', other:'' },
{ tag: 'fixed_value', title:'The Fixed Effects Value Density Function', other:' theta_j prior_id lambda_j mu_j delta_j d_j v_j^theta v^theta' },
{ tag: 'random_value', title:'The Random Effects Value Density Function', other:' u_j prior_id lambda_j mu_j delta_j d_j v_j^u v^u' },
{ tag: 'smooth_dage', title:'Prior Density Function for Smoothing Age Difference', other:' j lambda prior_ij d_ij delta_ij mu_ij v_ij a^s' },
{ tag: 'smooth_dtime', title:'Prior Density Function for Smoothing Time Difference', other:' j lambda prior_ij d_ij delta_ij mu_ij v_ij t^s' },
{ tag: 'fixed_diff', title:'Fixed Effects Differences Density Function', other:' k a^theta t^theta' },
{ tag: 'random_diff', title:'Random Effects Differences Density Function', other:' k a^u t^u' },
{ tag: 'fixed_prior', title:'Prior for Fixed Effect Values', other:' theta lambda beta constraints theta_k l_k^v u_k^v age difference limits a_i(k) delta^a l_k^a u_k^a time normalization constant c_theta p(theta)' },
{ tag: 'random_prior', title:'Prior for Random Effect Values', other:' p(u|theta)' },
{ tag: 'statistic', title:'Some Statistical Function Definitions', other:' notation z y mu delta eta nu log-transformed standard deviation sigma weighted residual value difference log-density uniform gaussian censored log-gaussian laplace log-laplace student\'s-t log-student\'s-t' },
{ tag: 'bilinear', title:'dismod_at Bilinear Interpolation', other:' age grid time function interpolant below minimum above maximum outside both limits plotting' },
{ tag: 'posterior', title:'Simulating Posterior Distribution for Model Variables', other:' purpose lemma 1 proof remark 2 simulation' },
{ tag: 'prev_dep', title:'Prevalence Does Not Depend On Other Cause Mortality', other:' lemma proof' },
{ tag: 'numeric_average', title:'Numerical Approximation of Average Integrands', other:' noise effect ode initial rectangle a_l a_u t_l t_u time step delta grid extended cohort solution line s_j lower cohorts upper refinement not required w(a t) g(a integration' },
{ tag: 'command', title:'The dismod_at Commands', other:' data flow log messages' },
{ tag: 'depend_command', title:'The Depend Command', other:' syntax database depend_var_table example' },
{ tag: 'depend_command.py', title:'depend Command: Example and Test', other:'' },
{ tag: 'fit_command', title:'The Fit Command', other:' syntax database variables fixed random both simulate_index data_sim_table prior_sim_table fit_var_table fit_data_subset_table effects convergence problems ipopt options example' },
{ tag: 'fit_command.py', title:'fit Command: Example and Test', other:'' },
{ tag: 'init_command', title:'The Initialize Command', other:' syntax purpose changing values database age_avg_table var_table data_subset_table start_var_table scale_var_table deleted tables example' },
{ tag: 'init_command.py', title:'init Command: Example and Test', other:'' },
{ tag: 'old2new_command', title:'Convert an Old Database to New Format', other:' syntax subgroup table avgint mulcov option' },
{ tag: 'predict_command', title:'The Predict Command', other:' syntax database source sample fit_var truth_var predict_table example' },
{ tag: 'predict_command.py', title:'predict Command: Example and Test', other:'' },
{ tag: 'sample_command', title:'The Sample Command', other:' syntax database purpose number_sample method simulate asymptotic data_sim_table prior_sim_table fit_var_table sample_table bounds example' },
{ tag: 'sample_command.py', title:'sample Command: Example and Test', other:'' },
{ tag: 'set_command', title:'Directly Setting Table Values', other:' syntax database option name init avgint table_out start_var scale_var truth_var source sample prior_mean fit_var example' },
{ tag: 'set_command.py', title:'set Command: Example and Test', other:' truth_var table' },
{ tag: 'simulate_command', title:'The Simulate Command', other:' syntax database number_simulate meas_value truth_var_table data_sim_table prior_sim_table example' },
{ tag: 'simulate_command.py', title:'simulate Command: Example and Test', other:'' },
{ tag: 'dismodat.py', title:'Python Program Syntax', other:' log table start end' },
{ tag: 'python', title:'Python Utilities', other:'' },
{ tag: 'create_connection', title:'Create a Python sqlite3 Database Connection', other:' syntax file_name new' },
{ tag: 'create_table', title:'Create a Database Table', other:' syntax connection tbl_name col_name unique col_type row_list tbl_name_id side effects example' },
{ tag: 'create_table.py', title:'create_table and Unicode: Example and Test', other:'' },
{ tag: 'create_database', title:'Create a Dismod_at Database', other:' syntax purpose primary key name column file_name age_list time_list integrand_table node_table subgroup_table backward compatibility weight_table covariate_table avgint_table covariates avgint_extra_columns data_table meas_std eta nu data_extra_columns prior_table smooth_table age_id time_id mulstd_value_prior_name mulstd_dage_prior_name mulstd_dtime_prior_name fun const_value nslist_table rate_table mulcov_table effected subsmooth option_table example' },
{ tag: 'create_database.py', title:'create_database: Example and Test', other:'' },
{ tag: 'csv2db_command', title:'Conversion of a Csv File to a Dismod_at Database', other:' syntax program python function see also example mtall mtother constraint age-time pairs rectangular grid hold_out rate predictions configure_csv non_zero_rates measure_csv integrand sincidence remission mtexcess mtwith susceptible withc prevalence tincidence mtspecific mtstandard relrisk age_lower age_upper time_lower time_upper meas_value meas_std' },
{ tag: 'db2csv_command', title:'Create Csv Files that Summarize The Database', other:' syntax program python function convention dir fit_var fit_data_subset simulate_index option.csv log.csv age_avg.csv variable.csv var_id var_type s_id m_id time rate integrand covariate node group subgroup fixed depend start scale fit_value truth sam_avg sam_std res_value res_dage res_dtime lag_value lag_dage lag_dtime sim_v sim_a sim_t prior_info data.csv data_id data_extra_columns child weight age_lo age_up time_lo time_up out density eta nu meas_std meas_stdcv meas_delta meas_value avgint residual sim_value sim_stdcv sim_delta covariates predict.csv avgint_id avgint_extra_columns s_index example ihme_db.sh' },
{ tag: 'db2csv_command.py', title:'db2csv Command: Example and Test', other:'' },
{ tag: 'ihme_db.sh', title:'Make a Local Copy an IHME Dismod_at Database', other:' syntax purpose download python3 absolute_dir --user relative_dir db2csv' },
{ tag: 'get_name_type', title:'Get Column Names and Types in a Table', other:' syntax connection table_name col_name col_type primary key example' },
{ tag: 'get_name_type.py', title:'get_name_type: Example and Test', other:'' },
{ tag: 'get_row_list', title:'Get Data From a Table', other:' syntax connection table_name col_name example' },
{ tag: 'get_row_list.py', title:'get_row_list: Example and Test', other:'' },
{ tag: 'get_table_dict', title:'Get All Data From a Table', other:' syntax connection tbl_name example' },
{ tag: 'get_table_dict.py', title:'get_table_dict: Example and Test', other:'' },
{ tag: 'metropolis', title:'Metropolis MCMC Algorithm', other:' syntax log_f x0 vector case float example' },
{ tag: 'user_metropolis.py', title:'Predict Average Integrand Using Results of a Fit', other:' source code' },
{ tag: 'modify_command', title:'Modify a Column of an Sqlite Database', other:' syntax program python function see also warning arg_list table_name column_name row_expression value_expression expressions statements log example' },
{ tag: 'modify_command.py', title:'modify Command: Example and Test', other:'' },
{ tag: 'sql_command', title:'Execute an SQL command', other:' syntax connection result' },
{ tag: 'system_command_prc', title:'Print Run and Check a System Command', other:' syntax purpose standard output error example' },
{ tag: 'unicode_tuple', title:'Convert an Iterable Object to a Unicode String', other:' syntax quote_string none infinity u_tuple example' },
{ tag: 'unicode_tuple.py', title:'unicode_tuple: Example and Test', other:'' },
{ tag: 'release_notes', title:'Changes and Additions to Dismod_at', other:' introduction this year previous years' },
{ tag: 'whats_new_2020', title:'Changes and Additions to Dismod_at During 2020', other:' 02-14 02-13 01-31 01-16 01-08 01-07' },
{ tag: 'whats_new_2019', title:'Changes and Additions to Dismod_at During 2019', other:' 12-07 12-06 12-05 12-04 11-30 11-29 11-28 11-26 11-25 11-24 11-22 11-20 11-18 11-17 10-24 10-09 10-08 10-06 10-02 10-01 09-30 09-24 09-23 09-22 09-21 09-20 09-19 09-17 09-15 09-13 09-12 09-11 08-24 07-25 07-24 07-20 07-19 07-13 07-10 07-06 06-28 06-27 06-26 06-25 06-24 06-23 06-22 06-21 06-07 06-02 06-01 05-24 05-21 05-12 05-07 05-05 05-03 05-02 05-01 04-30 04-29 04-28 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-18 04-17 04-15 04-14 04-12 04-11 04-10 04-09 04-08 04-06 02-16 01-26 01-23 01-22 01-16 01-15 01-10' },
{ tag: 'whats_new_2018', title:'Changes and Additions to Dismod_at During 2018', other:' 12-19 12-15 12-14 10-30 10-22 10-21 10-13 10-05 09-30 09-28 09-24 09-14 08-30 08-29 08-22 08-13 08-09 08-08 08-07 08-06 07-31 07-30 07-27 07-26 api other 07-25 07-23 07-21 07-13 06-30 06-29 06-21 06-20 06-18 06-14 06-09 06-04 05-24 05-23 05-21 05-14 05-09 05-08 05-07 05-05 05-04 05-02 04-30 04-26 04-25 04-24 04-23 04-22 04-21 04-16 04-13 04-12 04-11 04-10 04-09 04-08 04-07 04-06 03-21 03-13 03-10 03-08 03-06 02-20 02-17 02-14 02-13 02-12 02-11 02-09 02-08 02-05 02-04 02-03 02-02 02-01 01-31 01-30 01-28 01-26 01-25 01-24 01-23 01-22 01-21 01-20 01-18 01-16 01-12 01-09' },
{ tag: 'whats_new_2017', title:'Changes and Additions to Dismod_at During 2017', other:' 12-30 12-22 12-21 12-20 12-16 12-13 12-10 11-12 11-08 10-25 10-24 10-22 10-21 10-20 10-18 10-17 10-14 10-13 10-09 10-06 10-05 10-04 10-02 10-01 09-30 09-29 09-28 09-27 09-26 09-25 09-23 09-21 09-20 09-19 09-18 09-17 09-16 09-02 09-01 08-30 08-07 05-23 05-05 05-02 05-01 04-29 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-16 04-15 04-09 04-07 04-05 04-04 04-03 04-01 03-30 03-29 03-17 03-06 03-04 03-03 01-27 01-26 01-24 01-14 01-15' },
{ tag: 'whats_new_2016', title:'Changes and Additions to Dismod_at During 2016', other:' 12-03 10-31 10-20 10-19 10-18 10-17 10-16 10-15 10-14 10-13 10-12 10-11 09-30 09-29 09-21 08-28 07-29 07-28 07-15 07-14 07-11 06-13 06-12 06-06 06-05 05-16 05-15 05-11 05-10 05-09 05-08 05-07 05-06 05-05 05-04 05-03 05-01 04-28 04-27 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-11 04-09 04-07 03-29 02-26 02-06 01-26 01-16 01-15 01-10 01-06 01-05' },
{ tag: 'whats_new_2015', title:'Changes and Additions to Dismod_at During 2015', other:' 12-29 12-24 12-15 12-14 12-10 12-09 11-23 11-22 11-21 11-20 11-17 11-16 11-15 11-14 11-12 11-10 11-09 11-07 11-06 11-05 11-04 11-03 11-02 11-01 10-30 10-29 10-28 10-27 10-26 10-25 10-24 10-20 10-19 10-16 10-15 10-14 09-18 09-03 08-31 08-27 08-21 08-18 08-15 08-14 08-13 08-12 08-09 08-08 08-04 08-03 08-02 08-01 07-29 07-28 07-27 07-25 07-24 07-23 07-22 07-21 07-20 07-19 07-17 07-16 07-15 07-14 07-13 07-11 07-10 07-09 07-08 07-07 07-05 07-04 07-03 07-01 06-30 06-29 06-27 06-26 06-25 06-24 06-23 06-22' },
{ tag: 'wish_list', title:'Dismod_at Wish List', other:' simulating priors warm starting optimizer multi-threading asymptotic statistics user examples meas_std lagrange multipliers censored laplace create_database ode solution prevalence large excess mortality conditionals checkpointing command diagrams real world random tracing optimization windows install' }
]

var MaxList = 100;
var Nstring = -1;
var Nkeyword = Keyword.length;
var Row2Tag  = [];
Initialize();

function Initialize()
{
	UpdateList();
	document.search.keywords.focus();
}
function UpdateList()
{
	var string  = document.search.keywords.value;
	if( Nstring == string.length )
		return;
	Nstring     = string.length;

	var word    = string.match(/\S+/g);
	var nword   = 0;
	if(word != null )
		nword   = word.length;

	var pattern = new Array(nword);
	for(var j = 0; j < nword; j++)
		pattern[j] = new RegExp(word[j], 'i');

	var nlist       = 0;
	var title_list  = '';
	var tag_list    = '';
	for(i = 0; (i < Nkeyword) && (nlist < MaxList) ; i++)
	{
		var match = true;
		for(j = 0; j < nword; j++)
		{	var flag = pattern[j].test(Keyword[i].tag);
			flag     = flag || pattern[j].test(Keyword[i].title);
			flag     = flag || pattern[j].test(Keyword[i].other);
			match    = match && flag;
		}
		if( match )
		{
			var tag    = Keyword[i].tag;
			var title  = Keyword[i].title
			title      = title.split(/\s+/);
			title      = title.join(' ');
			title_list = title_list + title + '\n';
			tag_list   = tag_list + tag + '\n'
			Row2Tag[nlist] = tag;
			nlist = nlist + 1;
		}
	}
	document.search.title_list.value = title_list;
	document.search.title_list.setAttribute('wrap', 'off');;
	document.search.title_list.readOnly = true;
	document.search.tag_list.value = tag_list;
	document.search.tag_list.setAttribute('wrap', 'off');;
	document.search.tag_list.readOnly = true;
}
function Choose(textarea)
{	var start_select = textarea.value.substring(0, textarea.selectionStart);
	var start_pos    = Math.max(0, start_select.lastIndexOf('\n') + 1);
	var length       = textarea.value.length;
	var end_select   = 
		textarea.value.substring(textarea.selectionEnd, length);
	var end_pos  = end_select.indexOf('\n');
	end_pos      = textarea.selectionEnd + end_pos;
	textarea.selectionStart = start_pos;
	textarea.selectionEnd   = end_pos;
	var row = start_select.split('\n').length - 1;
	var tag = Row2Tag[row];
	document.search.selection.value    = tag.toLowerCase();
	document.search.selection.readOnly = true;
	
	return true;
}
function Goto()
{  selection       = document.search.selection.value;
   if( selection != '' )
	    parent.location = selection + '.htm';
}
function CheckForReturn()
{
	var key = event.which;
	if( key == 13 ) Goto();
}
