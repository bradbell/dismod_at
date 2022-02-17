// ------------------------------------------------------------ 
// Copyright (C) Bradley M. Bell 1998-2015, All rights reserved 
// ------------------------------------------------------------ 
Keyword = 
[
{ tag: 'dismod_at', title:'dismod_at-20220217: User Documentation and API', other:' purpose links' },
{ tag: 'math_abstract', title:'An Introduction To The Mathematics of Dismod_at', other:'' },
{ tag: 'math_ode', title:'The Dismod_at Ordinary Differential Equation', other:' susceptible s(a t) with condition c(a prevalence p(a incidence iota(a remission rho(a excess mortality chi(a other cause omega(a initial pini(t)' },
{ tag: 'censor_density', title:'The Censored Gaussian and Laplace Densities', other:' references discussion simulation test g(ymudelta) error function g(ymudeltac) difference between means l(ymudelta) indefinite integral l(ymudeltac)' },
{ tag: 'install', title:'Installing dismod_at', other:'' },
{ tag: 'dock_dismod_at.sh', title:'Install and Run dismod_at in a Docker Image', other:' syntax oci driver logging output purpose requirements building images version dismod_at.base dismod_at.mixed dismod_at.image removing containers dockerfile errors new build_type database command other arguments debugging determine id start stop' },
{ tag: 'install_unix', title:'Installing dismod_at in Unix', other:' system requirements c++ compiler fortran git cmake pkg-config python3 sqlite3 gsl suitesparse download dismod_at.git paths adding to pkg_config_path ld_library_path pythonpath special run_cmake.sh eigen ipopt cppad cppad_mixed setup command build test example' },
{ tag: 'run_cmake.sh', title:'bin/run_cmake.sh: User Configuration Options', other:' verbose_makefile build_type dismod_at_prefix debug release python3_executable specific_compiler extra_cxx_flags cmake_libdir system_specific_library_list' },
{ tag: 'example_install.sh', title:'An Example Installation', other:' syntax' },
{ tag: 'get_started', title:'Dismod_at Getting Started Examples / Tests', other:' see also database run one get_started.db commands' },
{ tag: 'get_started_db.py', title:'Create get_started Input Tables: Example and Test', other:' syntax discussion reference source code' },
{ tag: 'user_example', title:'User Examples and Tests', other:' see also run one user_speed.py example.db with explanation more' },
{ tag: 'user_age_avg_split.py', title:'Non-uniform Age Average Grid', other:' purpose variables prior data fit predict ode_step_size source code' },
{ tag: 'user_bilevel_random.py', title:'Example Fitting With Two Levels of Random Effects', other:' node table problem parameters model variables n1 n11 n12 n111 n112 n121 122 subgroup mulcov data procedure both compare truth sample posterior check coverage source code' },
{ tag: 'user_bnd_mulcov.py', title:'Bounding Covariate Multipliers Absolute Data Effect', other:' purpose integrands nodes true iota parent model max_cov_diff source code' },
{ tag: 'user_cascade.py', title:'Generating Priors For Next Level Down Node Tree', other:' table problem procedure step 1: create database 2: fit with n1 parent 3: simulate 4: sample posterior 5: predictions n11 6: 7: parameters age time values rate covariates multipliers gamma alpha source code' },
{ tag: 'user_censor.py', title:'Fitting Data That Has Negative Values Censored', other:' purpose problem parameters age time variables rate table prior discussion source code' },
{ tag: 'user_change_grid.py', title:'Remove an Age or Time From a Smoothing', other:' purpose table parent value prior difference fit source code' },
{ tag: 'user_compress.py', title:'Using Data Interval Compression', other:' purpose integrands nodes true iota model source code' },
{ tag: 'user_const_random.py', title:'Fitting With Non-Zero Constant Random Effects in Smoothing Grid', other:' source code' },
{ tag: 'user_const_value.py', title:'Constrain Omega Using const_value', other:' node table problem parameters child random effects priors nslist iota parent data random_seed source code' },
{ tag: 'user_continue_fit.py', title:'Continuing a Fit From Where it Left Off', other:' option table source code' },
{ tag: 'user_covid_19.py', title:'Model The Covid-19 Epidemic', other:' goal data file sir discussion dismod variables rates covariates predictions display fit source code' },
{ tag: 'user_csv2db.py', title:'csv2db_command: Example and Test', other:' using this discussion rate_true grids data predictions mtall source code' },
{ tag: 'user_data_density.py', title:'Fit With Outliers Using Data Density Command', other:' purpose problem parameters age time values variables table rate prior fitting source code' },
{ tag: 'user_data_sim.py', title:'Explanation of Simulated Data Table, data_sim', other:' see also purpose random effects priors iota other rates covariate multiplier subset meas_noise_effect notation before simulation delta sigma z source code' },
{ tag: 'user_diabetes.py', title:'An Example / Speed Test Fitting Simulated Diabetes Data', other:' running this omega rho covariates table multipliers truth parent rates child effects predict problem parameters mulcov_dict node_list integrand_list parent_age_grid child_age_grid parent_time_grid child_time_grid ode_step_size meas_cv meas_repeat fit_with_noise_in_data random_seed quasi_fixed tolerance_fixed derivative_test_fixed truth2start accept_rel_err source code' },
{ tag: 'user_diff_constraint.py', title:'Fitting with Constraints on Differences in Age and Time', other:' source code' },
{ tag: 'user_fit_fixed_both.py', title:'Fit Fixed First Then Both', other:' purpose discussion source code' },
{ tag: 'user_fit_meas_noise.py', title:'Group Measurement Noise Covariate Multipliers, Gamma', other:' purpose random effects iota other rates subgroup table data meas_noise_effect scaling source code' },
{ tag: 'user_fit_random.py', title:'Fitting Just Random Effects', other:' purpose discussion source code' },
{ tag: 'user_fit_sim.py', title:'Fitting Simulated Data Example', other:' parent iota child other rates covariate multiplier starting point scaling priors source code' },
{ tag: 'user_group_mulcov.py', title:'Using Group Covariate Multipliers', other:' see also purpose true value variables integrand node tables subgroup covariates simulated data rate source code' },
{ tag: 'user_hes_fixed_math.py', title:'Check the Hessian of the Fixed Effects Objective', other:' purpose reference notation problem parameters random likelihood gradient w.r.t. cross term optimal implicit function definition derivatives laplace approximation asymptotic statistics scaling source code' },
{ tag: 'user_hes_random.py', title:'Check the Hessian of the Random Effects Objective', other:' purpose reference notation problem parameters likelihood gradient w.r.t. asymptotic statistics source code' },
{ tag: 'user_hold_out.py', title:'Using hold_out in Data, Subset Data, and Option Tables', other:' purpose integrands nodes prevalence sincidence model fit source code' },
{ tag: 'user_ill_condition.py', title:'An Ill Conditioned Example Where Re-Scaling is Helpful', other:' notation objective derivative ill-conditioning source code' },
{ tag: 'user_jump_at_age.py', title:'Zero Rate Until a Jump at a Known Age', other:' purpose parameters iota_near_zero iota_after_20 iota_eta age_table time_table model variables truth simulated data source code' },
{ tag: 'user_lasso_covariate.py', title:'Using Lasso on Covariate Multiplier', other:' see also purpose problem parameters age time values variables table mulcov rate data prior iota alpha source code' },
{ tag: 'user_mulstd.py', title:'Estimating Smoothing Standard Deviation Multiplies', other:' purpose problem parameters age time values variables parent children data table rate prior fitting source code' },
{ tag: 'user_no_children.py', title:'Case with no Children; i.e., no Random Effects', other:' integrands nodes data outlier smoothing source code' },
{ tag: 'user_one_function.py', title:'Fitting One Function of Two Variables', other:' purpose rho random effects covariates simulated data density true prevalence computing delta prior random_seed source code' },
{ tag: 'user_plot_curve.py', title:'Example Plotting Log-Scaled Values w.r.t Age and Time', other:' z_name grid function' },
{ tag: 'user_plot_data_fit.py', title:'Example Plotting The Data and Its Fit', other:' nodes rates integrands n_data max_plot measurement noise hold_out call to source code' },
{ tag: 'user_plot_rate_fit.py', title:'Example Plotting The Rates for a Fit', other:' nodes integrands data call to source code' },
{ tag: 'user_predict_fit.py', title:'Predict Average Integrand Using Results of a Fit', other:' purpose note table problem parameters time values rate variables data avgint source code' },
{ tag: 'user_predict_mulcov.py', title:'Predict Covariate Multiplier Values', other:' purpose problem parameters age time rate table variables integrand data avgint source code' },
{ tag: 'user_re_scale.py', title:'Case Where Re-Scaling is Useful', other:' source code' },
{ tag: 'user_residual.py', title:'Weighted Residuals', other:' example parameters model data delta sigma gaussian log-gaussian priors value difference source code' },
{ tag: 'user_sample_asy.py', title:'Sample from Asymptotic Distribution for Model Variables', other:' purpose notation likelihood problem parameters source code' },
{ tag: 'user_sample_asy_sim.py', title:'Sampling From The Asymptotic Distribution for a Simulated Data Fit', other:' purpose discussion posterior variance source code' },
{ tag: 'user_sample_sim.py', title:'Sample Posterior Using Simulated Data', other:' purpose node table model variables iota x parameters delta values likelihood number_sample number_metropolis truth random_seed source code' },
{ tag: 'user_sim_log.py', title:'Simulating Data with Log Transformed Distribution', other:' see also example parameters model covariate multiplier notation sigma delta simulations source code' },
{ tag: 'user_speed.py', title:'A Simulate Data Speed Test', other:' syntax python3 random_seed n_children quasi_fixed ode_step_size effects iota_parent_true rho_parent_true mulcov_income_iota_true mulcov_sex_rho_true eta measure_cv n_data age_list time_list source code' },
{ tag: 'user_subgroup_mulcov.py', title:'Example Fitting With Subgroup Covariate Multipliers', other:' node table model variables parent rates child effects data problem parameters source code' },
{ tag: 'user_sum_residual.py', title:'Sum of Residuals at Optimal Estimate', other:' problem solution weighted cv standard deviations average source code' },
{ tag: 'user_system_command_prc.py', title:'Example Using system_command_prc', other:' discussion source code' },
{ tag: 'user_trace_init.py', title:'Using Initialization Trace Option', other:' purpose integrands nodes true iota model parent child data trace_init_fit_model source code' },
{ tag: 'user_warm_start.py', title:'Continuing a Fit Using Ipopt Warm Start', other:' option table fixed trace source code' },
{ tag: 'user_zsum_child_rate.py', title:'Constrain Sum of Child Rate Effect to Zero', other:' see also purpose problem parameters data simulation north_america canada united_states mexico nodes model variables source code' },
{ tag: 'user_zsum_mulcov_meas.py', title:'Constrain Sum of Subgroup Measurement Covariate Multipliers to Zero', other:' see also purpose problem parameters data simulation nodes model variables source code' },
{ tag: 'user_zsum_mulcov_rate.py', title:'Constrain Sum of Subgroup Rate Covariate Multipliers to Zero', other:' see also purpose problem parameters data simulation nodes model variables source code' },
{ tag: 'database', title:'The Dismod_at Database Tables', other:' comment columns primary key name foreign parent null bounds integer text can be not empty real nan type names' },
{ tag: 'input', title:'Tables That Are Only Used as Inputs', other:'' },
{ tag: 'age_table', title:'The Age Table', other:' age_id age_min age_max example' },
{ tag: 'age_table.py', title:'age_table: Example and Test', other:'' },
{ tag: 'avgint_table', title:'The avgint Table: Defines Average Integrand Cases', other:' see also purpose usage parent rates child avgint_id integrand_id node_id subgroup_id weight_id null age_lower age_upper time_lower time_upper covariates example' },
{ tag: 'avgint_table.py', title:'avgint_table: Example and Test', other:'' },
{ tag: 'covariate_table', title:'The Covariate Table', other:' discussion covariate_id covariate_name reference max_difference data subset avgint example' },
{ tag: 'covariate_table.py', title:'covariate_table: Example and Test', other:'' },
{ tag: 'data_table', title:'The Data Table', other:' discussion data_id data_name integrand_id density_id nonsmooth node_id parent child subgroup_id weight_id null hold_out meas_value meas_std eta age_lower age_upper time_lower time_upper covariates example' },
{ tag: 'data_table.py', title:'data_table: Example and Test', other:'' },
{ tag: 'density_table', title:'The Density Table', other:' discussion density_id density_name uniform gaussian cen_gaussian log_gaussian cen_log_gaussian laplace cen_laplace log_laplace cen_log_laplace students log_students notation linear censored scaled nonsmooth example' },
{ tag: 'density_table.py', title:'density_table: Example and Test', other:'' },
{ tag: 'integrand_table', title:'The Integrand Table', other:' discussion integrand_id minimum_meas_cv integrand_name ode names example' },
{ tag: 'integrand_table.py', title:'integrand_table: Example and Test', other:'' },
{ tag: 'mulcov_table', title:'The mulcov Table: Covariate Multipliers', other:' discussion mulcov_id mulcov_type rate_value meas_value meas_noise rate_id integrand_id covariate_id group_id group_smooth_id pini subgroup_smooth_id example' },
{ tag: 'mulcov_table.py', title:'mulcov_table: Example and Test', other:'' },
{ tag: 'node_table', title:'The Node Table', other:' discussion node_id node_name parent children descendant example' },
{ tag: 'node_table.py', title:'node_table: Example and Test', other:'' },
{ tag: 'nslist_pair_table', title:'Lists of Node Smoothing Pairs', other:' discussion nslist_pair_id nslist_id node_id smooth_id example' },
{ tag: 'nslist_pair_table.py', title:'nslist_pair_table: Example and Test', other:'' },
{ tag: 'nslist_table', title:'The Node Smoothing List Table', other:' discussion nslist_id nslist_name example' },
{ tag: 'nslist_table.py', title:'nslist_table: Example and Test', other:'' },
{ tag: 'option_table', title:'The Option Table', other:' format option_id option_name option_value default changing values parent node parent_node_id parent_node_name children data subset avgint zero sum constraints zero_sum_child_rate zero_sum_mulcov_group extra columns data_extra_columns avgint_extra_columns age average grid ode_step_size age_avg_split rate_case no_ode iota_zero_rho_zero iota_pos_rho_zero iota_zero_rho_pos iota_pos_rho_pos optimize fixed random derivative_test max_num_iter print_level accept_after_max_steps tolerance only quasi_fixed bound_frac_fixed limited_memory_max_history_fixed method_random bound_random meas_noise_effect warn_on_stderr minimum_meas_cv hold_out_integrand random_seed compress_interval trace_init_fit_model example' },
{ tag: 'option_default', title:'List of Option Default Values', other:' discussion table' },
{ tag: 'option_table.py', title:'option_table: Example and Test', other:'' },
{ tag: 'prior_table', title:'The Prior Table', other:' purpose prior_id prior_name density_id lower upper mean uniform std linear scaling log eta null fixed effects example' },
{ tag: 'prior_table.py', title:'prior_table: Example and Test', other:'' },
{ tag: 'rate_table', title:'The Rate Table', other:' discussion rate_id rate_name pini iota rho chi omega parent_smooth_id null child_smooth_id random effects hessian child_nslist_id value priors constant not difference example' },
{ tag: 'rate_table.py', title:'rate_table: Example and Test', other:'' },
{ tag: 'smooth_grid_table', title:'The Smooth Grid Table', other:' purpose smooth_grid_id smooth_id null age_id time_id value_prior_id dage_prior_id maximum dtime_prior_id const_value rectangular example' },
{ tag: 'smooth_grid_table.py', title:'smooth_grid_table: Example and Test', other:'' },
{ tag: 'smooth_table', title:'The Smoothing Table', other:' purpose empty smooth_id smooth_name n_age n_time mulstd_value_prior_id null mulstd_dage_prior_id mulstd_dtime_prior_id removing example' },
{ tag: 'subgroup_table', title:'The Subgroup Table', other:' purpose subgroup_id subgroup_name discussion spaces order example' },
{ tag: 'time_table', title:'The Time Table', other:' time_id ode grid example' },
{ tag: 'time_table.py', title:'time_table: Example and Test', other:'' },
{ tag: 'weight_grid_table', title:'The Weight Grid Table', other:' discussion weight_grid_id weight_id age_id time_id rectangular example' },
{ tag: 'weight_grid_table.py', title:'weight_grid_table: Example and Test', other:'' },
{ tag: 'weight_table', title:'The Weight Table', other:' weight_id weight_name n_age n_time example' },
{ tag: 'data_flow', title:'The Dismod_at Data Flow', other:' discussion output tables name command extra input' },
{ tag: 'age_avg_table', title:'The Age Average Table', other:' grid creation age_avg_id' },
{ tag: 'bnd_mulcov_table', title:'The Covariate and Multiplier Bound Table', other:' bnd_mulcov_id max_cov_diff max_mulcov' },
{ tag: 'data_sim_table', title:'Simulated Measurements and Adjusted Standard Deviations', other:' discussion data_sim_id simulate_index data_subset_id data_sim_value method data_id y delta eta sigma z example' },
{ tag: 'data_subset_table', title:'The Data Subset Table', other:' data_subset_id data_id node covariates hold_out density_id eta nu example' },
{ tag: 'depend_var_table', title:'Which Variables The Model Depends On', other:' purpose depend_var_id data_depend prior_depend example' },
{ tag: 'fit_data_subset_table', title:'The Model and Weighted Residuals Corresponding to a Fit', other:' discussion fit_data_subset_id avg_integrand weighted_residual example' },
{ tag: 'fit_var_table', title:'The Optimization Results for Variables', other:' discussion lagrange multipliers fit_var_id fit_var_value residual_value residual_dage residual_dtime lagrange_value lagrange_dage lagrange_dtime example' },
{ tag: 'hes_fixed_table', title:'Hessian of The Fixed Effect Objective Function', other:' creation scaling hes_fixed_id row_var_id col_var_id hes_fixed_value representation' },
{ tag: 'hes_random_table', title:'Hessian of The Random Effect Objective Function', other:' creation hes_random_id row_var_id col_var_id hes_random_value representation' },
{ tag: 'log_table', title:'The Log Table', other:' discussion log_id message_type command random_seed error warning table_name row_id unix_time' },
{ tag: 'mixed_info_table', title:'The CppAD Mixed Information Table', other:' discussion mixed_info_id mixed_name mixed_value n_fixed n_random quasi_fixed a_nr a_nnz ran_like_fun.size_var fix_like_fun.size_var' },
{ tag: 'predict_table', title:'The Predict Table: Average Integrand Predictions', other:' see also purpose avgint subset node covariates predict_id sample_index avgint_id avg_integrand example' },
{ tag: 'prior_sim_table', title:'Simulated Variation in Prior', other:' discussion prior_sim_id simulate_index var_id prior_sim_value zero_sum_child_rate zero_sum_mulcov_group prior_sim_dage prior_sim_dtime censoring example' },
{ tag: 'sample_table', title:'The Sample Table: Samples of Variable Values', other:' simulated data user\'s choice sample_id sample_index var_id var_value example' },
{ tag: 'scale_var_table', title:'Scaling Fixed Effects Objective and Constraints', other:' discussion scale_var_id scale_var_value example' },
{ tag: 'start_var_table', title:'Starting Values Used During a Fit', other:' discussion start_var_id start_var_value example' },
{ tag: 'trace_fixed_table', title:'The Fixed Effects Optimization Trace Table', other:' discussion trace_fixed_id iter obj_value inf_pr inf_du mu d_norm regularization_size alpha_du alpha_pr ls_trials restoration reference' },
{ tag: 'truth_var_table', title:'True Values Used During Simulations', other:' discussion truth_var_id truth_var_value example' },
{ tag: 'var_table', title:'Identifying Model Variables', other:' discussion var_id var_type mulstd_value mulstd_dage mulstd_dtime rate mulcov_rate_value mulcov_meas_value mulcov_meas_noise smooth_id age_id time_id node_id rate_id integrand_id covariate_id mulcov_id group_id otherwise subgroup_id example' },
{ tag: 'model', title:'The Age-Time Dismod Model', other:'' },
{ tag: 'model_variables', title:'The Model Variables', other:' introduction prior standard deviation multipliers functions age time children fixed effects theta smoothing lambda parent rates group covariate random subgroup variation initial prevalence other cases' },
{ tag: 'avg_integrand', title:'Model for the Average Integrand', other:' ordinary differential equation data avgint table notation a_i b_i c_i d_i s_i covariate difference x_ij w_i n_i rate functions parent q_k child effect u_ik j_k group multiplier alpha_jk subgroup delta adjusted r_ik pini p_i0(t) iota_i(at) rho_i(at) chi_i(at) omega_i(at) s_i(at) c_i(at) sincidence remission mtexcess mtother mtwith susceptible withc prevalence tincidence mtspecific mtall mtstandard relrisk mulcov measurement value covariates k_i beta_j weight integral wbar_i' },
{ tag: 'data_like', title:'Data Likelihood and Weighted Residuals', other:' notation a_i b_i d_i i_i s_i t_i w_i x_ij y_i minimum cv standard deviation delta_i transformed sigma_i fixed effects theta random average integrand a_i(u theta) measurement noise covariates k_i gamma_j (a t) e_i(theta) adjusted delta_i(theta) scaling add variance add_std_scale_none add_std_scale_log add_std_scale_all add_var_scale_none add_var_scale_log add_var_scale_all' },
{ tag: 'variable_prior', title:'Prior for the Model Variables', other:'' },
{ tag: 'fixed_value', title:'The Fixed Effects Value Density Function', other:' theta_j prior_id lambda_j mu_j epsilon_j d_j sigma_j v_j^theta v^theta' },
{ tag: 'random_value', title:'The Random Effects Value Density Function', other:' u_j prior_id lambda_j mu_j epsilon_j eta_j d_j sigma_j v_j^u v^u' },
{ tag: 'smooth_dage', title:'Prior Density Function for Smoothing Age Difference', other:' j lambda prior_ij d_ij sigma_ij mu_ij v_ij a^s' },
{ tag: 'smooth_dtime', title:'Prior Density Function for Smoothing Time Difference', other:' j lambda prior_ij d_ij sigma_ij mu_ij v_ij t^s' },
{ tag: 'fixed_diff', title:'Fixed Effects Differences Density Function', other:' k a^theta t^theta' },
{ tag: 'random_diff', title:'Random Effects Differences Density Function', other:' k a^u t^u' },
{ tag: 'fixed_prior', title:'Prior for Fixed Effect Values', other:' theta lambda beta constraints theta_k l_k^v u_k^v age difference limits a_i(k) delta^a l_k^a u_k^a time normalization constant c_theta p(theta)' },
{ tag: 'random_prior', title:'Prior for Random Effect Values', other:' p(u|theta)' },
{ tag: 'statistic', title:'Some Statistical Function Definitions', other:' notation y z mu delta eta nu weighted residual value difference log-density uniform gaussian censored log-gaussian laplace log-laplace student\'s-t log-student\'s-t' },
{ tag: 'bilinear', title:'dismod_at Bilinear Interpolation', other:' age grid time function interpolant below minimum above maximum outside both limits plotting' },
{ tag: 'posterior', title:'Simulating Posterior Distribution for Model Variables', other:' purpose lemma 1 proof remark 2 simulation' },
{ tag: 'prev_dep', title:'Prevalence Does Not Depend On Other Cause Mortality', other:' lemma proof' },
{ tag: 'numeric_average', title:'Numerical Approximation of Average Integrands', other:' noise effect ode initial rectangle a_l a_u t_l t_u time step delta extended grid cohort solution line s_j lower cohorts upper refinement not required w(a t) g(a integration' },
{ tag: 'command', title:'The dismod_at Commands', other:' data flow log messages' },
{ tag: 'dismodat.py', title:'Python Program Syntax', other:' log table start end' },
{ tag: 'bnd_mulcov_command', title:'Bound The Covariate Multiplier Absolute Data Effect', other:' syntax purpose database max_abs_effect covariate_name bnd_mulcov_table max_cov_diff max_mulcov infinite case example' },
{ tag: 'data_density_command', title:'Data Density Command: Change the Density for an Integrand', other:' syntax purpose database integrand_name eta_factor nu data_subset_table density_id example' },
{ tag: 'depend_command', title:'The Depend Command', other:' syntax database depend_var_table example' },
{ tag: 'depend_command.py', title:'depend Command: Example and Test', other:'' },
{ tag: 'fit_command', title:'The Fit Command', other:' syntax database variables fixed random both simulate_index data_sim_table prior_sim_table warm_start data_subset_table hold_out output tables fit_var_table fit_data_subset_table trace_fixed hes_random_table mixed_info ipopt_info_table effects convergence problems options example' },
{ tag: 'fit_command.py', title:'fit Command: Example and Test', other:'' },
{ tag: 'hold_out_command', title:'Hold Out Command: Randomly Sub-sample The Data', other:' syntax purpose database integrand_name max_fit balancing data_subset_table example' },
{ tag: 'init_command', title:'The Initialize Command', other:' syntax purpose deleted tables changing values database age_avg_table var_table data_subset_table hold_out density_id eta nu start_var_table scale_var_table bnd_mulcov_table example' },
{ tag: 'init_command.py', title:'init Command: Example and Test', other:'' },
{ tag: 'old2new_command', title:'Convert an Old Database to New Format', other:' syntax subgroup table avgint mulcov option' },
{ tag: 'predict_command', title:'The Predict Command', other:' syntax database source sample fit_var truth_var predict_table example' },
{ tag: 'predict_command.py', title:'predict Command: Example and Test', other:'' },
{ tag: 'sample_command', title:'The Sample Command', other:' syntax database purpose method variables number_sample simulate_index asymptotic fixed effects distribution random extra input tables data_sim_table prior_sim_table fit_var_table output sample_table no hes_fixed_table hes_random_table bounds example' },
{ tag: 'sample_command.py', title:'sample Command: Example and Test', other:'' },
{ tag: 'set_command', title:'Directly Setting Table Values', other:' syntax database option name init avgint table_out start_var scale_var truth_var source sample prior_mean fit_var example' },
{ tag: 'set_command.py', title:'set Command: Example and Test', other:' truth_var table' },
{ tag: 'simulate_command', title:'The Simulate Command', other:' syntax database number_simulate meas_value truth_var_table data_sim_table prior_sim_table example' },
{ tag: 'simulate_command.py', title:'simulate Command: Example and Test', other:'' },
{ tag: 'python', title:'Python Utilities', other:'' },
{ tag: 'average_integrand', title:'Compute The Average Integrand', other:' syntax purpose rate integrand_name grid time abs_tol avg_integrand example' },
{ tag: 'user_average_integrand.py', title:'Using the Python average_integrand Utility', other:' see also random effects priors simulation model data ode_step_size source code' },
{ tag: 'connection_file', title:'Get File Name For a Database Connection', other:' syntax purpose file_name example' },
{ tag: 'user_connection_file.py', title:'Example Using connection_file', other:' source code' },
{ tag: 'create_connection', title:'Create a Python sqlite3 Database Connection', other:' syntax file_name new' },
{ tag: 'create_database', title:'Create a Dismod_at Database', other:' syntax purpose primary key name column file_name age_list time_list integrand_table node_table subgroup_table backward compatibility weight_table covariate_table avgint_table covariates avgint_extra_columns data_table meas_std eta nu data_extra_columns prior_table smooth_table age_id time_id mulstd_value_prior_name mulstd_dage_prior_name mulstd_dtime_prior_name fun const_value nslist_table rate_table mulcov_table effected subsmooth option_table example' },
{ tag: 'create_database.py', title:'create_database: Example and Test', other:'' },
{ tag: 'create_table', title:'Create a Database Table', other:' syntax connection tbl_name col_name unique col_type row_list tbl_name_id side effects example' },
{ tag: 'create_table.py', title:'create_table and Unicode: Example and Test', other:'' },
{ tag: 'csv2db_command', title:'Conversion of a Csv File to a Dismod_at Database', other:' deprecated syntax program python function see also example mtall mtother constraint age-time pairs rectangular grid hold_out rate predictions configure_csv non_zero_rates measure_csv integrand sincidence remission mtexcess mtwith susceptible withc prevalence tincidence mtspecific mtstandard relrisk age_lower age_upper time_lower time_upper meas_value meas_std' },
{ tag: 'db2csv_command', title:'Create Csv Files that Summarize The Database', other:' syntax program python function convention dir fit_var fit_data_subset simulate_index option.csv log.csv age_avg.csv hes_fixed.csv hes_random.csv trace_fixed.csv mixed_info.csv variable.csv var_id var_type s_id m_id m_diff bound time rate integrand covariate node group subgroup depend fit_value start scale truth sam_avg sam_std res_value res_dage res_dtime lag_value lag_dage lag_dtime sim_v sim_a sim_t prior_info data.csv data_id data_extra_columns child weight age_lo age_up time_lo time_up d_out s_out density eta nu meas_std meas_stdcv meas_delta meas_value avgint residual sim_value covariates predict.csv avgint_id avgint_extra_columns s_index example ihme_db.sh' },
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
{ tag: 'modify_command', title:'Modify a Column of an Sqlite Database', other:' deprecated syntax program python function see also warning arg_list table_name column_name row_expression value_expression expressions statements log example' },
{ tag: 'modify_command.py', title:'modify Command: Example and Test', other:'' },
{ tag: 'perturb_command', title:'Perturb Command: Random Change to Start or Scale Tables', other:' syntax program python function database tbl_name scale_var start_var sigma seed' },
{ tag: 'plot_curve', title:'Plot Log-Scaled Values With Respect To Age and Time', other:' syntax pdf_file plot_title plot_limit age_min age_max time_min time_max plot_data std example' },
{ tag: 'plot_data_fit', title:'Plot The Data Fit By Integrand', other:' syntax database pdf_file plot_title max_plot integrand_list n_fit_dict legend table hold out point symbol plus green red black example' },
{ tag: 'plot_rate_fit', title:'Plot The Rates for a Fit', other:' syntax database pdf_file plot_title rate_set plot_set example' },
{ tag: 'replace_table', title:'Replace A a Table', other:' syntax connection table_name table_dict primary key example' },
{ tag: 'replace_table.py', title:'replace_table: Example and Test', other:'' },
{ tag: 'sql_command', title:'Execute an SQL command', other:' syntax connection result' },
{ tag: 'system_command_prc', title:'Print Run and Check a System Command', other:' syntax purpose print_command return_stdout return_stderr file_stdout file_stderr write_command result example' },
{ tag: 'unicode_tuple', title:'Convert an Iterable Object to a Unicode String', other:' syntax quote_string none infinity u_tuple example' },
{ tag: 'unicode_tuple.py', title:'unicode_tuple: Example and Test', other:'' },
{ tag: 'release_notes', title:'Changes and Additions to Dismod_at', other:' introduction this year previous years' },
{ tag: 'whats_new_2022', title:'Changes and Additions to Dismod_at During 2022', other:' 02-17 02-08 02-05 01-23 01-22' },
{ tag: 'whats_new_2021', title:'Changes and Additions to Dismod_at During 2021', other:' 12-31 12-30 12-27 12-22 12-14 12-09 12-08 12-05 11-29 11-27 11-19 11-16 11-15 11-14 11-07 11-06 11-05 11-03 10-31 10-25 10-24 10-23 10-15 10-14 10-12 10-11 10-07 10-06 09-01 08-30 08-25 08-24 08-23 08-22 08-21 08-18 08-17 08-15 08-12 08-08 07-29 07-25 07-24 07-22 07-14 07-05 07-04 07-02 06-24 06-22 06-18 06-17 06-15 06-14 06-13 06-12 06-11 06-10 06-09 06-08 06-07 06-06 06-01 05-31 05-29 05-21 05-20 05-19 05-18 05-15 05-07 05-06 04-16 03-02 01-28 01-22 01-21 01-19 01-11 01-10 01-09 01-08' },
{ tag: 'whats_new_2020', title:'Changes and Additions to Dismod_at During 2020', other:' 12-30 12-29 12-26 12-22 12-17 11-24 11-22 11-23 10-10 08-31 08-23 08-20 08-16 07-02 07-01 06-30 06-24 06-20 06-08 05-30 05-29 05-27 05-21 05-15 03-28 03-27 03-26 03-25 03-24 03-23 03-22 03-21 03-19 03-18 03-15 03-09 03-06 02-29 02-28 02-26 02-20 02-18 02-14 02-13 01-31 01-16 01-08 01-07' },
{ tag: 'whats_new_2019', title:'Changes and Additions to Dismod_at During 2019', other:' 12-07 12-06 12-05 12-04 11-30 11-29 11-28 11-26 11-25 11-24 11-22 11-20 11-18 11-17 10-24 10-09 10-08 10-06 10-02 10-01 09-30 09-24 09-23 09-22 09-21 09-20 09-19 09-17 09-15 09-13 09-12 09-11 08-24 07-25 07-24 07-20 07-19 07-13 07-10 07-06 06-28 06-27 06-26 06-25 06-24 06-23 06-22 06-21 06-07 06-02 06-01 05-24 05-21 05-12 05-07 05-05 05-03 05-02 05-01 04-30 04-29 04-28 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-18 04-17 04-15 04-14 04-12 04-11 04-10 04-09 04-08 04-06 02-16 01-26 01-23 01-22 01-16 01-15 01-10' },
{ tag: 'whats_new_2018', title:'Changes and Additions to Dismod_at During 2018', other:' 12-19 12-15 12-14 10-30 10-22 10-21 10-13 10-05 09-30 09-28 09-24 09-14 08-30 08-29 08-22 08-13 08-09 08-08 08-07 08-06 07-31 07-30 07-27 07-26 api other 07-25 07-23 07-21 07-13 06-30 06-29 06-21 06-20 06-18 06-14 06-09 06-04 05-24 05-23 05-21 05-14 05-09 05-08 05-07 05-05 05-04 05-02 04-30 04-26 04-25 04-24 04-23 04-22 04-21 04-16 04-13 04-12 04-11 04-10 04-09 04-08 04-07 04-06 03-21 03-13 03-10 03-08 03-06 02-20 02-17 02-14 02-13 02-12 02-11 02-09 02-08 02-05 02-04 02-03 02-02 02-01 01-31 01-30 01-28 01-26 01-25 01-24 01-23 01-22 01-21 01-20 01-18 01-16 01-12 01-09' },
{ tag: 'whats_new_2017', title:'Changes and Additions to Dismod_at During 2017', other:' 12-30 12-22 12-21 12-20 12-16 12-13 12-10 11-12 11-08 10-25 10-24 10-22 10-21 10-20 10-18 10-17 10-14 10-13 10-09 10-06 10-05 10-04 10-02 10-01 09-30 09-29 09-28 09-27 09-26 09-25 09-23 09-21 09-20 09-19 09-18 09-17 09-16 09-02 09-01 08-30 08-07 05-23 05-05 05-02 05-01 04-29 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-16 04-15 04-09 04-07 04-05 04-04 04-03 04-01 03-30 03-29 03-17 03-06 03-04 03-03 01-27 01-26 01-24 01-14 01-15' },
{ tag: 'whats_new_2016', title:'Changes and Additions to Dismod_at During 2016', other:' 12-03 10-31 10-20 10-19 10-18 10-17 10-16 10-15 10-14 10-13 10-12 10-11 09-30 09-29 09-21 08-28 07-29 07-28 07-15 07-14 07-11 06-13 06-12 06-06 06-05 05-16 05-15 05-11 05-10 05-09 05-08 05-07 05-06 05-05 05-04 05-03 05-01 04-28 04-27 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-11 04-09 04-07 03-29 02-26 02-06 01-26 01-16 01-15 01-10 01-06 01-05' },
{ tag: 'whats_new_2015', title:'Changes and Additions to Dismod_at During 2015', other:' 12-29 12-24 12-15 12-14 12-10 12-09 11-23 11-22 11-21 11-20 11-17 11-16 11-15 11-14 11-12 11-10 11-09 11-07 11-06 11-05 11-04 11-03 11-02 11-01 10-30 10-29 10-28 10-27 10-26 10-25 10-24 10-20 10-19 10-16 10-15 10-14 09-18 09-03 08-31 08-27 08-21 08-18 08-15 08-14 08-13 08-12 08-09 08-08 08-04 08-03 08-02 08-01 07-29 07-28 07-27 07-25 07-24 07-23 07-22 07-21 07-20 07-19 07-17 07-16 07-15 07-14 07-13 07-11 07-10 07-09 07-08 07-07 07-05 07-04 07-03 07-01 06-30 06-29 06-27 06-26 06-25 06-24 06-23 06-22' },
{ tag: 'wish_list', title:'Dismod_at Wish List', other:' speed bound constraints laplace random effect immunity simulating priors multi-threading user examples meas_std lagrange multipliers censored create_database ode solution prevalence large excess mortality rate_case command diagrams real world starting point windows install' }
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
