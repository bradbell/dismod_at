// ------------------------------------------------------------ 
// Copyright (C) Bradley M. Bell 1998-2015, All rights reserved 
// ------------------------------------------------------------ 
Keyword = 
[
'dismod_at  Disease Rates as Functions of Age and Time: dismod_at-20180214  ',' ',
'install_unix  Installing dismod_at in Unix  ',' system requirements c++ compiler fortran git cmake pkg-config python3 sqlite3 gsl download special run_cmake.sh eigen ipopt suitesparse cppad cppad_mixed setup command test executable path example ',
'run_cmake.sh  bin/run_cmake.sh: User Configuration Options  ',' verbose_makefile build_type dismod_at_prefix eigen_prefix other prefixes debug release python3_executable choosing c++ compiler extra_cxx_flags cmake_libdir checkpoint_newton_step system_specific_library_list ',
'example_install.sh  An Example Installation  ',' syntax ',
'get_started  Dismod_at Getting Started Examples / Tests  ',' see also database commands ',
'get_started_db.py  Create get_started Input Tables: Example and Test  ',' syntax purpose reference source code ',
'user  User Examples and Tests  ',' see also example.db ',
'user_asymptotic.py  Sample from Asymptotic Distribution for Model Variables  ',' purpose notation likelihood ',
'user_const_random.py  Fitting With Non-Zero Constant Random Effects in Smoothing Grid  ',' ',
'user_const_value.py  Fitting Using const_value in Smoothing Grid  ',' ',
'user_continue_fit.py  Continuing a Fit From Where it Left Off  ',' option table ',
'user_diabetes.py  An Example / Speed Test Fitting Simulated Diabetes Data  ',' running this omega rho weight table grid covariates multipliers truth predict problem parameters mulcov_dict node_list integrand_list parent_age_grid child_age_grid parent_time_grid child_time_grid ode_step_size meas_cv meas_repeat fit_with_noise_in_data random_seed quasi_fixed tolerance_fixed derivative_test_fixed truth2start accept_rel_err source code ',
'user_diff_constraint.py  Constraints on Differences in Age and Time  ',' ',
'user_fit_fixed.py  Fitting Just Fixed Effects  ',' discussion ',
'user_fit_random.py  Fitting Just Random Effects  ',' discussion ',
'user_fit_sim.py  Fitting Simulated Data Example  ',' ',
'user_jump_at_age.py  Case with a Jump in Rate at an Age  ',' ',
'user_lasso_covariate.py  Using Lasso on Covariate Multiplier  ',' see also ',
'user_meas_covariate.py  Using Measurement Covariates on Multiple Integrands  ',' see also ',
'user_no_children.py  Case with no Children; i.e., no Random Effects  ',' ',
'user_predict_fit.py  Predict Average Integrand Using Results of a Fit  ',' ',
'user_zero_sum_random.py  Fitting With Sum of Random Effect Constrained to Zero  ',' discussion ',
'user_re_scale.py  Case Where Re-Scaling is Useful  ',' ',
'user_speed.py  A Simulate Data Speed Test  ',' syntax python3 random_seed n_children n_data_per_child quasi_fixed ',
'user_students.py  Using Student\'s-t to Fitting Data with Outliers  ',' ',
'database  The Dismod_at Database Tables  ',' comment columns primary key name foreign parent text can be null not empty integer real nan bounds that ',
'input  Tables That Are Only Used as Inputs  ',' ',
'age_table  The Age Table  ',' age_id ode grid example ',
'age_table.py  age_table: Example and Test  ',' ',
'avgint_table  The avgint Table: Defines Average Integrand Cases  ',' see also purpose usage avgint_id integrand_id node_id weight_id age_lower age_upper time_lower time_upper covariates example ',
'avgint_table.py  avgint_table: Example and Test  ',' ',
'covariate_table  The Covariate Table  ',' covariate_id covariate_name reference max_difference data subset avgint example ',
'covariate_table.py  covariate_table: Example and Test  ',' ',
'data_table  The Data Table  ',' data_id data_name integrand_id density_id laplace node_id parent random effects child weight_id hold_out meas_value meas_std eta null age_lower age_upper time_lower time_upper covariates example ',
'data_table.py  data_table: Example and Test  ',' ',
'density_table  The Density Table  ',' density_id density_name uniform gaussian laplace students log_gaussian log_laplace log_students example ',
'density_table.py  density_table: Example and Test  ',' ',
'integrand_table  The Integrand Table  ',' integrand_id integrand_name ode names example ',
'integrand_table.py  integrand_table: Example and Test  ',' ',
'mulcov_table  The mulcov Table: Covariate Multipliers  ',' mulcov_id mulcov_type rate_value meas_value meas_std rate_id integrand_id covariate_id smooth_id null pini example ',
'mulcov_table.py  mulcov_table: Example and Test  ',' ',
'node_table  The Node Table  ',' node_id node_name parent child group descendant example ',
'node_table.py  node_table: Example and Test  ',' ',
'nslist_pair_table  Lists of Node Smoothing Pairs  ',' nslist_pair_id nslist_id node_id smooth_id example ',
'nslist_pair_table.py  nslist_pair_table: Example and Test  ',' ',
'nslist_table  The Node Smoothing List Table  ',' nslist_id nslist_name example ',
'nslist_table.py  nslist_table: Example and Test  ',' ',
'option_table  The Option Table  ',' conventions changing values option_id option_name option_value parent_node_id parent_node_name default children data subset avgint zero_sum_random data_extra_columns avgint_extra_columns warn_on_stderr minimum_meas_cv ode_step_size random_seed rate_case iota_zero_rho_zero iota_pos_rho_zero iota_zero_rho_pos iota_pos_rho_pos optimizer quasi_fixed derivative_test max_num_iter print_level accept_after_max_steps tolerance bound_random bound_frac_fixed example ',
'option_table.py  option_table: Example and Test  ',' ',
'prior_table  The Prior Table  ',' prior_id prior_name density_id lower upper mean uniform std eta null scaling fixed effects example ',
'prior_table.py  prior_table: Example and Test  ',' ',
'rate_table  The Rate Table  ',' rate_id rate_name pini iota rho chi omega parent_smooth_id null child_smooth_id random effects hessian child_nslist_id restrictions constant density lower upper bound_random example ',
'rate_table.py  rate_table: Example and Test  ',' ',
'smooth_grid_table  The Smooth Grid Table  ',' smooth_grid_id smooth_id age_id time_id value_prior_id dage_prior_id dtime_prior_id const_value rectangular example ',
'smooth_grid_table.py  smooth_grid_table: Example and Test  ',' ',
'smooth_table  The Smoothing Table  ',' smooth_id smooth_name n_age n_time mulstd_value_prior_id null mulstd_dage_prior_id mulstd_dtime_prior_id example ',
'time_table  The Time Table  ',' time_id ode grid example ',
'time_table.py  time_table: Example and Test  ',' ',
'weight_grid_table  The Weight Grid Table  ',' weight_grid_id weight_id age_id time_id rectangular example ',
'weight_grid_table.py  weight_grid_table: Example and Test  ',' ',
'weight_table  The Weight Table  ',' weight_id weight_name n_age n_time example ',
'data_flow  The Dismod_at Data Flow  ',' output tables input command ',
'log_table  The Log Table  ',' log_id message_type command random_seed error warning table_name row_id unix_time ',
'var_table  Identifying Model Variables  ',' var_id var_type mulstd_value mulstd_dage mulstd_dtime rate mulcov_rate_value mulcov_meas_value mulcov_meas_std smooth_id age_id time_id node_id rate_id integrand_id covariate_id example ',
'data_subset_table  The Data Subset Table  ',' data_subset_id data_id discussion node covariates example ',
'depend_var_table  Which Variables The Model Depends On  ',' purpose depend_var_id data_depend prior_depend example ',
'start_var_table  Starting Values Used During a Fit  ',' discussion start_var_id start_var_value example ',
'scale_var_table  Scaling Fixed Effects Objective and Constraints  ',' discussion scale_var_id scale_var_value example ',
'fit_var_table  The Optimization Results for Variables  ',' lagrange multipliers fit_var_id fit_var_value residual_value residual_dage residual_dtime lagrange_value lagrange_dage lagrange_dtime example ',
'fit_data_subset_table  The Model and Weighted Residuals Corresponding to a Fit  ',' fit_data_subset_id avg_integrand weighted_residual example ',
'truth_var_table  True Values Used During Simulations  ',' discussion truth_var_id truth_var_value example ',
'simulate_table  The Simulated Table: Samples Simulated Measurements  ',' simulate_id simulate_index data_subset_id simulate_value simulate_delta example ',
'sample_table  The Sample Table: Samples of Variable Values  ',' simulated data user\'s choice sample_id sample_index var_id var_value example ',
'predict_table  The Predict Table: Average Integrand Predictions  ',' see also purpose avgint subset node covariates predict_id sample_index avgint_id avg_integrand example ',
'model  The Age-Time Dismod Model  ',' ',
'model_variables  The Model Variables  ',' prior smoothing interpolation number children fixed effects theta standard deviation multipliers lambda parent rates covariate random age time variation initial prevalence other types ',
'avg_integrand  Model for the Average Integrand  ',' ordinary differential equation data avgint table notation a_i b_i s_i t_i x_ij w_i n_i rate functions q_k u_ik j_k alpha_jk r_ik p_i0(t) pini iota_i(at) rho_i(at) chi_i(at) omega_i(at) s_i(at) c_i(at) sincidence remission mtexcess mtother mtwith susceptible withc prevalence tincidence mtspecific mtall mtstandard relrisk measurement value covariates k_i beta_j effect weight integral wbar_i ',
'data_like  Data Likelihood and Weighted Residuals  ',' table notation y_i delta_i b_i s_i t_i d_i i_i x_ij w_i measurement standard deviation covariates k_i gamma_j effects theta average integrand adjusted ',
'fixed_value  The Fixed Effects Value Density Function  ',' theta_j prior_id lambda_j mu_j delta_j d_j v_j v^theta ',
'fixed_diff  Fixed Effects Differences Density Function  ',' k i(k) j(k) lambda_k theta_ijk mu_ijk delta_ijk d_ijk age a^theta time t^theta ',
'fixed_prior  Prior for Fixed Effect Values  ',' theta lambda beta constraints theta_k l_k^v u_k^v age difference limits a_i(k) delta^a l_k^a u_k^a time normalization constant c_theta p(theta) ',
'random_value  The Random Effects Value Density Function  ',' j k_j a_j t_j u_cj lambda_j delta_j d_j mu_j v_cj^u v^u ',
'random_diff  Random Effects Difference Density Function  ',' k i(k) j(k) lambda_k u_ijkc delta_ijk d_ijk mu_ijk age differences a^u time t^u ',
'random_prior  Prior for Random Effect Values  ',' p(u|theta) ',
'statistic  Some Statistical Function Definitions  ',' notation log-transformed standard deviation sigma weighted residual difference uniform gaussian laplace student\'s-t log-gaussian log-laplace log-student\'s-t log-density ',
'bilinear  dismod_at Bilinear Interpolation  ',' age grid time function interpolant below minimum above maximum outside both limits plotting ',
'ode_grid  The dismod_at Ode Grid  ',' purpose delta a_min a_max t_min t_max number age points a_i time t_j ',
'command  The dismod_at Commands  ',' data flow log messages ',
'set_command  Directly Setting Table Values  ',' syntax database option warning table_out start_var scale_var truth_var source prior_mean fit_var sample example ',
'init_command  The Variable Command  ',' syntax purpose changing values database var_table data_subset_table start_var_table scale_var_table example ',
'depend_command  The Depend Command  ',' syntax database depend_var_table example ',
'fit_command  The Fit Command  ',' syntax database variables fixed random both simulate_index simulate_table fit_var_table fit_data_subset_table effects convergence problems example ',
'simulate_command  The Simulate Command  ',' syntax database number_simulate meas_value truth_var_table simulate_table simulate_value simulate_delta sigma linear log-transformed example ',
'sample_command  The Sample Command  ',' syntax database number_sample method simulate asymptotic simulate_table fit_var_table sample_table bounds example ',
'predict_command  The Predict Command  ',' syntax database source sample fit_var truth_var predict_table example ',
'set_command.py  variable Command: Example and Test  ',' truth_var table ',
'init_command.py  init Command: Example and Test  ',' ',
'depend_command.py  fit Command: Example and Test  ',' ',
'fit_command.py  fit Command: Example and Test  ',' ',
'simulate_command.py  simulate Command: Example and Test  ',' ',
'sample_command.py  sample Command: Example and Test  ',' ',
'predict_command.py  predict Command: Example and Test  ',' ',
'python  Python Utilities  ',' ',
'create_connection  Create a Python sqlite3 Database Connection  ',' syntax file_name new ',
'create_table  Create a Database Table  ',' syntax connection tbl_name col_name unique col_type row_list tbl_name_id side effects example ',
'create_table.py  create_table and Unicode: Example and Test  ',' ',
'create_database  Create a Dismod_at Database  ',' syntax purpose primary key name column file_name age_list time_list integrand_list node_table weight_table covariate_table avgint_table avgint_extra_columns data_table data_extra_columns prior_table smooth_table age_id time_id mulstd_value_prior_name mulstd_dage_prior_name mulstd_dtime_prior_name fun const_value nslist_table rate_table mulcov_table option_table example ',
'create_database.py  create_database: Example and Test  ',' ',
'db2csv_command  Create Csv Files that Summarize The Database  ',' syntax program python function convention dir option.csv log.csv variable.csv var_id var_type s_id age time rate integrand covariate node fixed start depend truth sam_avg sam_std fit_value res_value res_dage res_dtime lag_value lag_dage lag_dtime prior_info data.csv data_id data_extra_columns child weight age_lo age_up time_lo time_up out density meas_value meas_std eta nu delta avgint residual sim_value sim_delta covariates predict.csv avgint_id avgint_extra_columns s_index example ',
'db2csv_command.py  init Command: Example and Test  ',' ',
'get_name_type  Get Column Names and Types in a Table  ',' syntax connection table_name col_name col_type primary key example ',
'get_name_type.py  get_name_type: Example and Test  ',' ',
'get_row_list  Get Data From a Table  ',' syntax connection table_name col_name example ',
'get_row_list.py  get_row_list: Example and Test  ',' ',
'get_table_dict  Get All Data From a Table  ',' syntax connection tbl_name example ',
'get_table_dict.py  get_table_dict: Example and Test  ',' ',
'metropolis  Metropolis MCMC Algorithm  ',' syntax log_f x0 vector case float example ',
'user_metropolis.py  Predict Average Integrand Using Results of a Fit  ',' ',
'modify_command  Modify a Column of an Sqlite Database  ',' syntax program python function see also warning arg_list table_name column_name row_expression value_expression expressions statements example ',
'modify_command.py  init Command: Example and Test  ',' ',
'unicode_tuple  Convert an Iterable Object to a Unicode String  ',' from syntax quote_string none infinity u_tuple example ',
'unicode_tuple.py  unicode_tuple: Example and Test  ',' ',
'sql_command  Execute an SQL command  ',' syntax connection ',
'devel  Dismod_at Developer Documentation  ',' ',
'devel_table  Dismod_at Developer Table Documentation  ',' ',
'check_child_prior  Check Priors in Child Smoothing  ',' syntax purpose db rate_table smooth_grid nslist_pair prior_table ',
'check_pini_n_age  Check Initial Prevalence Grid Has One Age  ',' syntax db rate_table smooth_table ',
'check_rate_limit  Check Lower and Upper Iota and Rho Limits  ',' syntax db rate_case rate_table prior_table smooth_grid ',
'check_table_id  C++: Check That table_id Column Has Proper Values  ',' syntax purpose db table_name n_row speed ',
'check_zero_sum  Check Only One Smoothing for Each Rate in zero_sum_random  ',' syntax purpose db rate_table option_table ',
'cpp_create_table  Use C++ to Create a Database Table  ',' syntax purpose db table_name col_name col_type col_unique row_value single quote null table_name_id example ',
'create_table_xam.cpp  C++ create_table: Example and Test  ',' ',
'exec_sql_cmd  C++: Execution of an SQL Command  ',' execute syntax purpose db error ',
'get_age_table  C++: Get the Age Table Information  ',' syntax purpose db example ',
'get_age_table_xam.cpp  C++ get_age_table: Example and Test  ',' ',
'get_avgint_table  C++: Get the Average Integrand Case Table  ',' syntax purpose db n_covariate age_min age_max time_min time_max avgint_struct avgint_cov_value example ',
'get_avgint_table_xam.cpp  C++ get_avgint_table: Example and Test  ',' ',
'get_column_max  C++: Get The Maximum Values in a Table Column  ',' syntax db select_cmd column_name max_str example ',
'get_column_max_xam.cpp  C++ get_column_max: Example and Test  ',' ',
'get_covariate_table  C++: Get the Covariate Table Information  ',' syntax purpose db covariate_struct example ',
'get_covariate_table_xam.cpp  C++ get_covariate_table: Example and Test  ',' ',
'get_data_table  C++: Get the Data Table Information  ',' syntax purpose db n_covariate age_min age_max time_min time_max data_struct data_cov_value example ',
'get_data_table_xam.cpp  C++ get_data_table: Example and Test  ',' ',
'get_db_input  C++: Get the Data Base Input Tables  ',' syntax purpose primary key initial prevalence grid check rate limits child priors zero sum constraint ',
'get_density_table  C++: Get the Density Table Information  ',' syntax purpose db density_enum log_density example ',
'get_density_table_xam.cpp  C++ get_density_table: Example and Test  ',' ',
'get_integrand_table  C++: Get the Integrand Table Information  ',' syntax purpose db integrand_enum integrand_enum2name example ',
'get_integrand_table_xam.cpp  C++ get_integrand_table: Example and Test  ',' ',
'get_mulcov_table  C++: Get the Covariate Multiplier Table Information  ',' syntax purpose db mulcov_struct mulcov_type_enum example ',
'get_mulcov_table_xam.cpp  C++ get_mulcov_table: Example and Test  ',' ',
'get_node_table  C++: Get the Node Table Information  ',' syntax purpose db node_struct example ',
'get_node_table_xam.cpp  C++ get_node_table: Example and Test  ',' ',
'get_option_table  C++: Get the Option Table Information  ',' syntax purpose db option_struct special note example ',
'get_option_table_xam.cpp  C++ get_option_table: Example and Test  ',' ',
'get_prior_table  C++: Get the Prior Table Information  ',' syntax purpose db prior_struct checks example ',
'get_prior_table_xam.cpp  C++ get_prior_table: Example and Test  ',' ',
'get_rate_table  C++: Get the Rate Table Information  ',' syntax get_rate_name db rate_struct rate_enum example ',
'get_rate_table_xam.cpp  C++ get_rate_table: Example and Test  ',' ',
'get_sample_table  C++: Get the Simulate Table  ',' syntax purpose db sample_struct example ',
'get_sample_table_xam.cpp  C++ get_sample_table: Example and Test  ',' ',
'get_simulate_table  C++: Get the Simulate Table  ',' syntax purpose db simulate_struct example ',
'get_simulate_table_xam.cpp  C++ get_simulate_table: Example and Test  ',' ',
'get_smooth_grid  C++: Get the Smoothing Grid Information  ',' syntax purpose db smooth_grid_struct check example ',
'get_smooth_grid_xam.cpp  C++ get_smooth_grid: Example and Test  ',' ',
'get_nslist_table  C++: Get the nslist Table Information  ',' syntax purpose db example ',
'get_nslist_table_xam.cpp  C++ get_nslist_table: Example and Test  ',' ',
'get_nslist_pair  C++: Get the List of Smoothing Table  ',' syntax purpose db nslist_pair_struct example ',
'get_nslist_pair_xam.cpp  C++ get_nslist_pair_table: Example and Test  ',' ',
'get_smooth_table  C++: Get the Smoothing Table Information  ',' syntax purpose db smooth_struct example ',
'get_table_column  C++: Get The Type and Values in a Table Column  ',' syntax db table_name column_name column_type result text integer real example ',
'get_table_column_xam.cpp  C++ get_table_column_xam: Example and Test  ',' ',
'get_time_table  C++: Get the Time Table Information  ',' syntax purpose db example ',
'get_time_table_xam.cpp  C++ get_time_table: Example and Test  ',' ',
'get_weight_grid  C++: Get the Weight Grid Information  ',' syntax purpose db weight_grid_struct example ',
'get_weight_grid_xam.cpp  C++ get_weight_grid: Example and Test  ',' ',
'get_weight_table  C++: Get the Weight Table Information  ',' syntax purpose db weight_struct example ',
'log_message  Put a Message in the Log Table  ',' syntax db os message_type table_name row_id unix_time example ',
'put_table_row  C++: Insert New Row at End of a Table  ',' syntax db table_name col_name_vec row_val_vec null primary_key table_name_id example ',
'put_table_row_xam.cpp  C++ put_table_row: Example and Test  ',' ',
'open_connection  Open An Sqlite3 Database Connection  ',' syntax file_name new_file db close ',
'smooth_info  Extract and Organize Information for One Smoothing  ',' syntax constructors assignment member functions purpose assumptions s_info age_table time_table smooth_id smooth_table smooth_grid_table s_test age_id time_id type_prior_id mulstd_type s_default n_age n_time j a_id t_id c_value i_type m_type example ',
'smooth_info_xam.cpp  C++ smooth_info: Example and Test  ',' ',
'weight_info  Extract and Organize Information for One Weighting Function  ',' syntax purpose w_info age_table time_table weight_id weight_table weight_grid_table w_test age_id time_id w_default n_age n_time j a_id example ',
'weight_info_xam.cpp  C++ weight_info: Example and Test  ',' weight_grid ',
'devel_utility  Dismod_at Developer Utility Documentation  ',' ',
'ad_types  AD Types  ',' syntax purpose source code ',
'avgint_subset  Create a Subsampled Version of Average Integrand Case Table  ',' syntax see also purpose avgint_table avgint_cov_value covariate_table child_object avgint_subset_obj n_subset subset_id original_id avgint_subset_cov_value example ',
'avgint_subset_xam.cpp  C++ avgint_subset: Example and Test  ',' ',
'bilinear_interp  Bilinear Interpolation  ',' interpolate syntax x_grid y_grid z_grid j example ',
'bilinear_interp_xam.cpp  C++ bilinear_interp: Example and Test  ',' ',
'child_info  Child Indices and Tables Indices  ',' syntax child_object constructor parent_node_id node_table child_size n_child child_id2node_id table_id2child ',
'data_subset  Create a Subsampled Version of Data Table  ',' syntax see also purpose data_table data_cov_value covariate_table child_object data_subset_obj n_subset subset_id original_id data_subset_cov_value example ',
'data_subset_xam.cpp  C++ data_subset: Example and Test  ',' ',
'eigen_ode2  Eigen Vector Solution of ODE with Two Components  ',' syntax purpose float b assumption yi tf yf case_number three four method example ',
'eigen_ode2_xam.cpp  Example and Test of Eigenvector Method of Solving ODE  ',' ',
'error_exit  Printing and Logging Fatal Error Messages  ',' syntax db null table_name row_id assert ',
'fixed_effect  Setting and Getting the Fixed Effect Vector  ',' syntax float order effects pack_object n_fixed pack_index unpack_fixed pack_vec fixed_vec example ',
'fixed_effect_xam.cpp  C++ fixed_effect: Example and Test  ',' ',
'integrate_1d  Compute One Dimensional Integration Coefficients  ',' syntax see also purpose w example method ',
'integrate_1d_xam.cpp  C++ integrate_1d: Example and Test  ',' ',
'integrate_2d  Compute Two Dimensional Integration Coefficients  ',' syntax see also purpose b example method ',
'integrate_2d_xam.cpp  C++ integrate_1d: Example and Test  ',' ',
'interp_weight  Interpolate Weight Grid  ',' syntax age time w_info age_table time_table i_wi j_wi example ',
'interp_weight_xam.cpp  C++ interp_weight: Example and Test  ',' ',
'pack_info  Devel Packing Variables into a Single Vector  ',' example ',
'pack_info_ctor  Devel Variable Packing Information: Constructor  ',' syntax n_integrand child_id2node_id smooth_table mulcov_table rate_table nslist_pair pack_copy size integrand_size child_size smooth_size random_size age-time order example ',
'pack_info_mulstd  Devel Pack Variables: Standard Deviations Multipliers  ',' syntax mulstd_offset pack_object smooth_id example ',
'pack_info_rate_info  Devel Pack Variables: Rates  ',' syntax subvec_info pack_object rate_id covariate_id smooth_id n_var offset example ',
'pack_info_mulcov_meas  Devel Pack Variables: Measurement Covariate Multipliers  ',' syntax meas_value meas_std subvec_info pack_object integrand_id n_cov covariate_id smooth_id n_var offset example ',
'pack_info_mulcov_rate  Devel Pack Variables: Rate Covariate Multipliers  ',' syntax subvec_info pack_object rate_id n_cov covariate_id smooth_id n_var offset example ',
'pack_info_xam.cpp  C++ pack_info: Example and Test  ',' ',
'pack_value_prior  Prior Ids In Same Order As Packed Variables  ',' syntax pack_object s_info_vec value_prior_id_out const_value_out example ',
'pack_diff_prior  Difference Priors For Packed Variables  ',' syntax pack_object s_info_vec example ',
'pack_value_prior_xam.cpp  C++ pack_value_prior: Example and Test  ',' ',
'pack_diff_prior_xam.cpp  C++ pack_diff_prior: Example and Test  ',' ',
'residual_density  Compute Weighted Residual and Log-Density  ',' syntax float z mu delta d_id d_eta d_nu index difference residual_struct wres logden ',
'random_number  Pseudo Random Number Generation  ',' ',
'manage_gsl_rng  Set, Get, And Free A GSL Random Number Generator  ',' syntax purpose new_gsl_rng s_in s_out get_gsl_rng free_gsl_rng example ',
'manage_gsl_rng_xam.cpp  Manage GSL Random Number Generator: Example and Test  ',' ',
'sim_random  Simulate a Dismod_at Random Distribution  ',' syntax manage_gsl_rng density delta eta nu z example ',
'sim_random_xam.cpp  Manage GSL Random Number Generator: Example and Test  ',' ',
'random_effect  Setting and Getting the Random Effect Vector  ',' syntax float order effects pack_object pack_index unpack_random pack_vec random_vec example ',
'random_effect_xam.cpp  C++ random_effect: Example and Test  ',' ',
'number_random_const  Determine Number of Random Effects that are Constant  ',' syntax bound_random pack_object s_info_vec prior_table ',
'smooth2ode  Interpolation from Smoothing to Ode Grid  ',' syntax float n_age_ode n_time_ode ode_step_size age_table time_table s_info interpolate var_si ode_index var_ode example ',
'smooth2ode_xam.cpp  C++ smooth2ode: Example and Test  ',' ',
'solve_ode  Solving the Dismod Ordinary Differential Equation  ',' syntax 2do purpose notation float i_max j_max step_size pini rate s_out c_out example ',
'solve_ode_xam.cpp  C++ solve_ode: Example and Test  ',' ',
'split_space  Split String Into Substrings With Spaces as Delimiter  ',' syntax vec example prototype ',
'split_space_xam.cpp  C++ split_space: Example and Test  ',' ',
'devel_model  Dismod_at Developer Model Documentation  ',' ',
'devel_data_model  Model for Data Given Model Variables  ',' ',
'data_model_ctor  Data Model: Constructor  ',' syntax data_object bound_random parent_node_id n_covariate n_age_ode n_time_ode ode_step_size age_table time_table integrand_table node_table subset_object subset_cov_value w_info_vec s_info_vec pack_object child_object n_covariate_ n_age_ode_ n_time_ode_ ode_step_size_ pack_object_ minimum_meas_cv_ eigen_ode2_case_number_ replace_like_called_ n_child_ data_subset_obj_ si2ode_vec_ child_ran_zero_ data_info_ ',
'set_eigne_ode2_case_number  Sets the Method Used to Solve the Dismod_at ODE  ',' syntax data_object rate_case ',
'data_model_replace_like  Set Value Necessary for Likelihood (not for Average Integrand)  ',' syntax purpose data_object minimum_meas_cv subset_object n_subset subset_id data_subset_obj laplace density ',
'data_model_reference_ode  Solve for S and C Corresponding to Reference Covariate Values  ',' syntax prototype data_object float pack_vec parent_only reference_sc ',
'data_model_sci_ode  Use the ODE to Integrand for one Measurement  ',' syntax prototype float data_object i_min j_min n_age_sub n_time_sub child pack_vec reference_sc sci_sub example ',
'data_model_avg_no_ode  One Average Integrand That Doesn\'t Require the ODE  ',' syntax data_object float subset_id rates pack_vec example ',
'data_model_avg_yes_ode  One Average Integrand That Requires the ODE  ',' syntax data_object float subset_id pack_vec reference_sc example ',
'data_model_like_one  One Weighted Residual and Log-Likelihood for any Integrands  ',' syntax requirement data_object float subset_id pack_vec avg delta_out index example ',
'data_model_like_all  All the Weighted Residuals and Log-Likelihoods  ',' syntax requirement data_object float hold_out random_depend pack_vec residual_vec index include outs no density ',
'sci_ode_xam.cpp  C++ sci_ode: Example and Test  ',' ',
'avg_no_ode_xam.cpp  C++ avg_no_ode: Example and Test  ',' ',
'avg_yes_ode_xam.cpp  C++ avg_yes_ode: Example and Test  ',' ',
'like_one_xam.cpp  C++ like_one: Example and Test  ',' ',
'prior_model  Prior Model for Model Variables  ',' ',
'prior_model_ctor  Construct Fixed Negative Log-Likelihood Object  ',' syntax prior_object pack_object age_table time_table prior_table s_info_vec ',
'prior_fixed_effect  Evaluate Fixed Negative Log-Likelihood for the Fixed Effects  ',' syntax float prior_object pack_vec residual_vec p(theta) example ',
'prior_random_effect  Evaluate Fixed Negative Log-Likelihood for the Random Effects  ',' syntax float prior_object pack_vec residual_vec p(u|theta) example ',
'prior_fixed_xam.cpp  C++ prior_fixed: Example and Test  ',' ',
'prior_random_xam.cpp  C++ prior_random: Example and Test  ',' ',
'fit_model  Object that Fits Fixed and Random Effects Model to Data  ',' ',
'fit_model_ctor  Fit Model Constructor  ',' syntax fit_object db bound_random fit_or_sample pack_object start_var scalar_var prior_table s_info_vec data_object prior_object quasi_fixed zero_sum_random prototype ',
'fit_model_run_fit  Run optimization to determine the optimal fixed and random effects  ',' syntax fit_object scaling random_only option_map prototype ',
'fit_model_get_solution  Get Solution Corresponding to Previous Fit  ',' syntax fit_object fit_var_value lagrange_value lagrange_dage lagrange_dtime example prototype ',
'fit_model_sample_posterior  Sample From Posterior Distribution for a Fit  ',' syntax fit_object fit_var_value lagrange_value lagrange_dage lagrange_dtime prototype ',
'fit_model_xam.cpp  C++ fit_model: Example and Test  ',' ',
'depend  Compute Data or Prior Dependencies  ',' ',
'data_depend  Which Variables The Data Model Depends On  ',' syntax prototype pack_vec data_object ',
'prior_depend  Which Variables The Prior Depends On  ',' syntax prototype pack_vec prior_object ',
'whats_new_2018  Changes and Additions to Dismod_at During 2018  ',' 02-14 02-13 02-12 02-11 02-09 02-08 02-05 02-04 02-03 02-02 02-01 01-31 01-30 01-28 01-26 01-25 01-24 01-23 01-22 01-21 01-20 01-18 01-16 01-12 01-09 ',
'whats_new_2017  Changes and Additions to Dismod_at During 2017  ',' 12-30 12-22 12-21 12-20 12-16 12-13 12-10 11-12 11-08 10-25 10-24 10-22 10-21 10-20 10-18 10-17 10-14 10-13 10-09 10-06 10-05 10-04 10-02 10-01 09-30 09-29 09-28 09-27 09-26 09-25 09-23 09-21 09-20 09-19 09-18 09-17 09-16 09-02 09-01 08-30 08-07 05-23 05-05 05-02 05-01 04-29 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-16 04-15 04-09 04-07 04-05 04-04 04-03 04-01 03-30 03-29 03-17 03-06 03-04 03-03 01-27 01-26 01-24 01-14 01-15 ',
'whats_new_2016  Changes and Additions to Dismod_at During 2016  ',' 12-03 10-31 10-20 10-19 10-18 10-17 10-16 10-15 10-14 10-13 10-12 10-11 09-30 09-29 09-21 08-28 07-29 07-28 07-15 07-14 07-11 06-13 06-12 06-06 06-05 05-16 05-15 05-11 05-10 05-09 05-08 05-07 05-06 05-05 05-04 05-03 05-01 04-28 04-27 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-11 04-09 04-07 03-29 02-26 02-06 01-26 01-16 01-15 01-10 01-06 01-05 ',
'whats_new_2015  Changes and Additions to Dismod_at During 2015  ',' 12-29 12-24 12-15 12-14 12-10 12-09 11-23 11-22 11-21 11-20 11-17 11-16 11-15 11-14 11-12 11-10 11-09 11-07 11-06 11-05 11-04 11-03 11-02 11-01 10-30 10-29 10-28 10-27 10-26 10-25 10-24 10-20 10-19 10-16 10-15 10-14 09-18 09-03 08-31 08-27 08-21 08-18 08-15 08-14 08-13 08-12 08-09 08-08 08-04 08-03 08-02 08-01 07-29 07-28 07-27 07-25 07-24 07-23 07-22 07-21 07-20 07-19 07-17 07-16 07-15 07-14 07-13 07-11 07-10 07-09 07-08 07-07 07-05 07-04 07-03 07-01 06-30 06-29 06-27 06-26 06-25 06-24 06-23 06-22 ',
'wish_list  Dismod_at Wish List  ',' ode solution step size large excess mortality conditionals checkpointing lagrange multipliers documentation real world example user examples model simulating data sampling random effects special hessian starting tracing optimization windows install '
]

var MaxList = 100;
var Nstring = -1;
var Nkeyword = Keyword.length / 2;
Initialize();

function Initialize()
{
	UpdateList();
	document.search.keywords.focus();
}
function UpdateList(event)
{
	key = 0;
	if( window.event )
		key = window.event.keyCode;
	else if( event )
		key = event.which;
	if( key == 13 )
	{	Goto();
		return;
	}
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

	var nlist = 0;
	var list  = '';
	for(i = 0; (i < Nkeyword) && (nlist < MaxList) ; i++)
	{
		var match = true;
		for(j = 0; j < nword; j++)
		{	var flag = pattern[j].test(Keyword[2*i]);
			flag     = flag || pattern[j].test(Keyword[2*i+1]);
			match    = match && flag;
		}

		if( match )
		{
			line  = Keyword[2*i].split(/\s+/);
			line  = line.join(' ');
			list  = list + line + '\n';
			nlist = nlist + 1;
		}
	}
	document.search.list.value    = list;
}
function Choose(textarea)
{	var start_select = textarea.value.substring(0, textarea.selectionStart);
	var start_pos    = Math.max(0, start_select.lastIndexOf('\n') );
	var length       = textarea.value.length;
	var end_select   = 
		textarea.value.substring(textarea.selectionEnd, length);
	var end_pos      = end_select.indexOf('\n');
	if( end_pos >= 0 ) 
	{	end_pos = textarea.selectionEnd + end_pos;
	} else 
	{	end_pos = length;
	}
	// highlight the selected line
	textarea.selectionStart = start_pos;
	textarea.selectionEnd   = end_pos;
	// get the choice from the beginning of the line
	var line = textarea.value.substring(start_pos, length);
	var end_choice = line.indexOf(' ');
	if( end_choice >= 0 )
	{	var choice = line.substring(0, end_choice);
		document.search.choice.value = choice.toLowerCase();
	}
	
	return true;
}
function Goto()
{
parent.location = document.search.choice.value + '.htm';
}
