// ------------------------------------------------------------ 
// Copyright (C) Bradley M. Bell 1998-2015, All rights reserved 
// ------------------------------------------------------------ 
Keyword = 
[
'dismod_at  dismod_at-20190920: User Documentation and API  ',' purpose links ',
'math_abstract  An Introduction To The Mathematics of Dismod_at  ',' ',
'math_ode  The Dismod_at Ordinary Differential Equation  ',' susceptible s(a t) with condition c(a prevalence p(a incidence iota(a remission rho(a excess mortality chi(a other cause omega(a initial pini(t) ',
'censor_density  The Censored Gaussian and Laplace Densities  ',' references discussion simulation test g(ymudelta) error function g(ymudeltac) difference between means l(ymudelta) indefinite integral l(ymudeltac) ',
'install  Installing dismod_at  ',' ',
'dock_dismod_at.sh  Install and Run dismod_at in a Docker Image  ',' syntax logging output purpose requirements building images version dismod_at.base dismod_at.image removing containers dockerfile errors new build_type other arguments debugging determine id start stop ',
'install_unix  Installing dismod_at in Unix  ',' system requirements c++ compiler fortran git cmake pkg-config python3 sqlite3 gsl suitesparse download dismod_at.git paths adding to pkg_config_path ld_library_path pythonpath special run_cmake.sh eigen ipopt cppad cppad_mixed setup command build test example ',
'run_cmake.sh  bin/run_cmake.sh: User Configuration Options  ',' verbose_makefile build_type dismod_at_prefix eigen_prefix ipopt_prefix cppad_prefix debug release python3_executable choosing c++ compiler extra_cxx_flags cmake_libdir system_specific_library_list ',
'example_install.sh  An Example Installation  ',' syntax ',
'get_started  Dismod_at Getting Started Examples / Tests  ',' see also database run one get_started.db commands ',
'get_started_db.py  Create get_started Input Tables: Example and Test  ',' syntax discussion reference source code ',
'user  User Examples and Tests  ',' see also run one user_speed.py example.db with explanation more ',
'user_csv2db.py  csv2db_command: Example and Test  ',' using this discussion rate_true grids data predictions mtall source ',
'user_fit_random.py  Fitting Just Random Effects  ',' purpose discussion source code ',
'user_fit_fixed_both.py  Fit Fixed First Then Both  ',' purpose discussion source code ',
'user_posterior.py  Sampling From Then Posterior Distribution  ',' purpose discussion variance source code ',
'user_fit_sim.py  Fitting Simulated Data Example  ',' parent iota child other rates covariate multiplier starting point scaling priors source code ',
'user_fit_gamma.py  Fitting Measurement Noise Covariate Multipliers, Gamma  ',' random effects iota other rates data meas_noise_effect scaling source code ',
'user_age_avg_split.py  Non-uniform Age Average Grid  ',' purpose variables prior data fit predict ode_step_size source code ',
'user_diabetes.py  An Example / Speed Test Fitting Simulated Diabetes Data  ',' running this omega rho covariates table multipliers truth parent rates child effects predict problem parameters mulcov_dict node_list integrand_list parent_age_grid child_age_grid parent_time_grid child_time_grid ode_step_size meas_cv meas_repeat fit_with_noise_in_data random_seed quasi_fixed tolerance_fixed derivative_test_fixed truth2start accept_rel_err source code ',
'user_ill_condition.py  An Ill Conditioned Example Where Re-Scaling is Helpful  ',' notation objective derivative ill-conditioning source code ',
'user_jump_at_age.py  Jump in Rate Value at a Known Age  ',' purpose model variables truth simulated data ages table time source code ',
'user_change_grid.py  Remove an Age or Time From a Smoothing  ',' purpose table parent value prior difference fit source code ',
'user_lasso_covariate.py  Using Lasso on Covariate Multiplier  ',' see also purpose problem parameters age time values variables table mulcov rate data prior iota alpha source code ',
'user_students.py  Using Student\'s-t to Fit With Remove Outliers Present  ',' purpose problem parameters age time values variables data table rate prior fitting source code ',
'user_censor.py  Fitting Data That Has Negative Values Censored  ',' purpose problem parameters age time variables rate table prior discussion source code ',
'user_cascade.py  Generating Priors For Next Level Down Node Tree  ',' table problem procedure step 1: create database 2: fit with n1 parent 3: simulate 4: sample posterior 5: predictions n11 6: 7: parameters age time values rate covariates multipliers gamma alpha source code ',
'user_predict_fit.py  Predict Average Integrand Using Results of a Fit  ',' purpose note table problem parameters time values rate variables data avgint source code ',
'user_predict_mulcov.py  Predict Covariate Multiplier Values  ',' purpose problem parameters age time rate table variables integrand data avgint source code ',
'user_data_sim.py  Explanation of Simulated Data Table, data_sim  ',' random effects priors iota other rates covariate multiplier subset meas_noise_effect scaling gamma source code ',
'user_sample_sim.py  Sample Posterior Using Simulated Data  ',' purpose node table model variables iota x parameters delta values likelihood number_sample number_metropolis truth source code ',
'user_sample_asy.py  Sample from Asymptotic Distribution for Model Variables  ',' purpose notation likelihood source code ',
'user_const_value.py  Constrain Omega Using const_value  ',' node table problem parameters child random effects priors nslist iota parent data source code ',
'user_const_random.py  Fitting With Non-Zero Constant Random Effects in Smoothing Grid  ',' source code ',
'user_continue_fit.py  Continuing a Fit From Where it Left Off  ',' option table source code ',
'user_diff_constraint.py  Constraints on Differences in Age and Time  ',' source code ',
'user_meas_covariate.py  Using Measurement Covariates on Multiple Integrands  ',' see also source code ',
'user_no_children.py  Case with no Children; i.e., no Random Effects  ',' source code ',
'user_zero_sum_random.py  Fitting With Sum of Random Effect Constrained to Zero  ',' discussion source code ',
'user_re_scale.py  Case Where Re-Scaling is Useful  ',' source code ',
'user_speed.py  A Simulate Data Speed Test  ',' syntax python3 random_seed n_children quasi_fixed ode_step_size source code ',
'database  The Dismod_at Database Tables  ',' comment columns primary key name foreign parent text can be null not empty integer real nan bounds that type names ',
'input  Tables That Are Only Used as Inputs  ',' ',
'age_table  The Age Table  ',' age_id average grid example ',
'age_table.py  age_table: Example and Test  ',' ',
'avgint_table  The avgint Table: Defines Average Integrand Cases  ',' see also purpose usage parent rates child avgint_id integrand_id node_id weight_id null age_lower age_upper time_lower time_upper covariates example ',
'avgint_table.py  avgint_table: Example and Test  ',' ',
'covariate_table  The Covariate Table  ',' covariate_id covariate_name reference max_difference data subset avgint example ',
'covariate_table.py  covariate_table: Example and Test  ',' ',
'data_table  The Data Table  ',' data_id data_name integrand_id density_id nonsmooth node_id parent random effects child weight_id null hold_out meas_value meas_std adjusted standard deviation delta linear scaling log eta age_lower age_upper time_lower time_upper covariates example ',
'data_table.py  data_table: Example and Test  ',' ',
'density_table  The Density Table  ',' density_id density_name uniform gaussian cen_gaussian log_gaussian cen_log_gaussian laplace cen_laplace log_laplace cen_log_laplace students log_students notation linear censored scaled nonsmooth example ',
'density_table.py  density_table: Example and Test  ',' ',
'integrand_table  The Integrand Table  ',' integrand_id minimum_meas_cv integrand_name ode names example ',
'integrand_table.py  integrand_table: Example and Test  ',' ',
'mulcov_table  The mulcov Table: Covariate Multipliers  ',' mulcov_id mulcov_type rate_value meas_value meas_noise meas_std: deprecated 2019-04-07 rate_id integrand_id covariate_id smooth_id null pini example ',
'mulcov_table.py  mulcov_table: Example and Test  ',' ',
'node_table  The Node Table  ',' node_id node_name parent child group descendant example ',
'node_table.py  node_table: Example and Test  ',' ',
'nslist_pair_table  Lists of Node Smoothing Pairs  ',' nslist_pair_id nslist_id node_id smooth_id example ',
'nslist_pair_table.py  nslist_pair_table: Example and Test  ',' ',
'nslist_table  The Node Smoothing List Table  ',' nslist_id nslist_name example ',
'nslist_table.py  nslist_table: Example and Test  ',' ',
'option_table  The Option Table  ',' conventions changing values option_id option_name option_value parent_node_id default parent_node_name children data subset avgint meas_noise_effect meas_std_effect: deprecated 2019-04-07 zero_sum_random data_extra_columns avgint_extra_columns warn_on_stderr minimum_meas_cv ode_step_size age_avg_split average grid random_seed rate_case iota_zero_rho_zero iota_pos_rho_zero iota_zero_rho_pos iota_pos_rho_pos fixed derivative_test max_num_iter print_level accept_after_max_steps tolerance only quasi_fixed bound_frac_fixed limited_memory_max_history_fixed method_random bound_random example ',
'option_table.py  option_table: Example and Test  ',' ',
'prior_table  The Prior Table  ',' purpose prior_id prior_name density_id lower upper mean uniform std linear scaling log discussion eta null fixed effects example ',
'prior_table.py  prior_table: Example and Test  ',' ',
'rate_table  The Rate Table  ',' rate_id rate_name pini iota rho chi omega parent_smooth_id null child_smooth_id random effects hessian child_nslist_id value priors constant not difference example ',
'rate_table.py  rate_table: Example and Test  ',' ',
'smooth_grid_table  The Smooth Grid Table  ',' purpose smooth_grid_id smooth_id null age_id time_id value_prior_id dage_prior_id dtime_prior_id const_value rectangular example ',
'smooth_grid_table.py  smooth_grid_table: Example and Test  ',' ',
'smooth_table  The Smoothing Table  ',' purpose empty smooth_id smooth_name n_age n_time mulstd_value_prior_id null mulstd_dage_prior_id mulstd_dtime_prior_id removing example ',
'time_table  The Time Table  ',' time_id ode grid example ',
'time_table.py  time_table: Example and Test  ',' ',
'weight_grid_table  The Weight Grid Table  ',' weight_grid_id weight_id age_id time_id rectangular example ',
'weight_grid_table.py  weight_grid_table: Example and Test  ',' ',
'weight_table  The Weight Table  ',' weight_id weight_name n_age n_time example ',
'data_flow  The Dismod_at Data Flow  ',' output tables input command ',
'log_table  The Log Table  ',' log_id message_type command random_seed error warning table_name row_id unix_time ',
'var_table  Identifying Model Variables  ',' var_id var_type mulstd_value mulstd_dage mulstd_dtime rate mulcov_rate_value mulcov_meas_value mulcov_meas_noise smooth_id age_id time_id node_id rate_id integrand_id covariate_id mulcov_id example ',
'data_subset_table  The Data Subset Table  ',' data_subset_id data_id discussion node covariates example ',
'depend_var_table  Which Variables The Model Depends On  ',' purpose depend_var_id data_depend prior_depend example ',
'start_var_table  Starting Values Used During a Fit  ',' discussion start_var_id start_var_value example ',
'scale_var_table  Scaling Fixed Effects Objective and Constraints  ',' discussion scale_var_id scale_var_value example ',
'age_avg_table  The Age Average Grid  ',' creation age_avg_id ',
'fit_var_table  The Optimization Results for Variables  ',' lagrange multipliers fit_var_id fit_var_value residual_value residual_dage residual_dtime lagrange_value lagrange_dage lagrange_dtime example ',
'fit_data_subset_table  The Model and Weighted Residuals Corresponding to a Fit  ',' fit_data_subset_id avg_integrand weighted_residual example ',
'truth_var_table  True Values Used During Simulations  ',' discussion truth_var_id truth_var_value example ',
'prior_sim_table  Simulated Variation in Prior  ',' discussion prior_sim_id simulate_index var_id prior_sim_value zero_sum_random prior_sim_dage prior_sim_dtime censoring example ',
'data_sim_table  Simulated Measurements and Adjusted Standard Deviations  ',' data_sim_id simulate_index data_subset_id data_sim_value data_sim_stdcv data_sim_delta method data_id y eta sigma z linear log scaled example ',
'sample_table  The Sample Table: Samples of Variable Values  ',' simulated data user\'s choice sample_id sample_index var_id var_value example ',
'predict_table  The Predict Table: Average Integrand Predictions  ',' see also purpose avgint subset node covariates predict_id sample_index avgint_id avg_integrand example ',
'model  The Age-Time Dismod Model  ',' ',
'model_variables  The Model Variables  ',' introduction function age time smoothing interpolation children fixed effects theta standard deviation multipliers lambda parent rates covariate random variation initial prevalence other groups ',
'avg_integrand  Model for the Average Integrand  ',' ordinary differential equation data avgint table notation a_i b_i s_i t_i covariate difference x_ij w_i n_i rate functions parent q_k child effect u_ik j_k multiplier alpha_jk adjusted r_ik pini p_i0(t) iota_i(at) rho_i(at) chi_i(at) omega_i(at) s_i(at) c_i(at) sincidence remission mtexcess mtother mtwith susceptible withc prevalence tincidence mtspecific mtall mtstandard relrisk mulcov measurement value covariates k_i beta_j weight integral wbar_i ',
'data_like  Data Likelihood and Weighted Residuals  ',' table notation y_i a_i b_i s_i t_i d_i i_i x_ij w_i minimum cv standard deviation delta_i measurement noise covariates k_i gamma_j effects theta average integrand adjusted scaling add variance add_std_scale_all add_std_scale_log add_var_scale_all add_var_scale_log ',
'fixed_value  The Fixed Effects Value Density Function  ',' theta_j prior_id lambda_j mu_j delta_j d_j v_j v^theta ',
'fixed_diff  Fixed Effects Differences Density Function  ',' k i(k) j(k) lambda_k theta_ijk mu_ijk delta_ijk d_ijk age a^theta time t^theta ',
'fixed_prior  Prior for Fixed Effect Values  ',' theta lambda beta constraints theta_k l_k^v u_k^v age difference limits a_i(k) delta^a l_k^a u_k^a time normalization constant c_theta p(theta) ',
'random_value  The Random Effects Value Density Function  ',' j k_j a_j t_j u_cj lambda_j delta_j d_j mu_j v_cj^u v^u ',
'random_diff  Random Effects Difference Density Function  ',' k i(k) j(k) lambda_k u_ijkc delta_ijk d_ijk mu_ijk age differences a^u time t^u ',
'random_prior  Prior for Random Effect Values  ',' p(u|theta) ',
'statistic  Some Statistical Function Definitions  ',' notation z y mu delta eta nu log-transformed standard deviation sigma weighted residual value difference log-density uniform gaussian censored log-gaussian laplace log-laplace student\'s-t log-student\'s-t ',
'bilinear  dismod_at Bilinear Interpolation  ',' age grid time function interpolant below minimum above maximum outside both limits plotting ',
'posterior  Simulating Posterior Distribution for Model Variables  ',' purpose lemma 1 proof remark 2 simulation ',
'prev_dep  Prevalence Does Not Depend On Other Cause Mortality  ',' lemma proof ',
'numeric_average  Numerical Approximation of Average Integrands  ',' noise effect ode initial rectangle a_l a_u t_l t_u time step delta grid extended cohort solution line s_j lower cohorts upper refinement not required w(a t) g(a integration ',
'command  The dismod_at Commands  ',' data flow log messages ',
'depend_command  The Depend Command  ',' syntax database depend_var_table example ',
'depend_command.py  fit Command: Example and Test  ',' ',
'fit_command  The Fit Command  ',' syntax database variables fixed random both simulate_index data_sim_table prior_sim_table fit_var_table fit_data_subset_table effects convergence problems ipopt options example ',
'fit_command.py  fit Command: Example and Test  ',' ',
'init_command  The Initialize Command  ',' syntax purpose changing values database age_avg_table var_table data_subset_table start_var_table scale_var_table deleted tables example ',
'init_command.py  init Command: Example and Test  ',' ',
'old2new_command  Convert an Old Database to New Format  ',' syntax discussion var table ',
'predict_command  The Predict Command  ',' syntax database source sample fit_var truth_var predict_table example ',
'predict_command.py  predict Command: Example and Test  ',' ',
'sample_command  The Sample Command  ',' syntax database purpose number_sample method simulate asymptotic data_sim_table prior_sim_table fit_var_table sample_table bounds example ',
'sample_command.py  sample Command: Example and Test  ',' ',
'set_command  Directly Setting Table Values  ',' syntax database option name init avgint table_out start_var scale_var truth_var source sample prior_mean fit_var example ',
'set_command.py  variable Command: Example and Test  ',' truth_var table ',
'simulate_command  The Simulate Command  ',' syntax database number_simulate meas_value truth_var_table data_sim_table prior_sim_table example ',
'simulate_command.py  simulate Command: Example and Test  ',' ',
'dismodat.py  Python Program Syntax  ',' log table start end ',
'python  Python Utilities  ',' ',
'create_connection  Create a Python sqlite3 Database Connection  ',' syntax file_name new ',
'create_table  Create a Database Table  ',' syntax connection tbl_name col_name unique col_type row_list tbl_name_id side effects example ',
'create_table.py  create_table and Unicode: Example and Test  ',' ',
'create_database  Create a Dismod_at Database  ',' syntax purpose primary key name column file_name age_list time_list integrand_table node_table weight_table covariate_table avgint_table covariates avgint_extra_columns data_table data_extra_columns prior_table smooth_table age_id time_id mulstd_value_prior_name mulstd_dage_prior_name mulstd_dtime_prior_name fun const_value nslist_table rate_table mulcov_table option_table example ',
'create_database.py  create_database: Example and Test  ',' ',
'csv2db_command  Conversion of a Csv File to a Dismod_at Database  ',' syntax program python function see also example mtall mtother constraint age-time pairs rectangular grid hold_out rate predictions configure_csv non_zero_rates measure_csv integrand sincidence remission mtexcess mtwith susceptible withc prevalence tincidence mtspecific mtstandard relrisk age_lower age_upper time_lower time_upper meas_value meas_std ',
'db2csv_command  Create Csv Files that Summarize The Database  ',' syntax program python function convention dir fit_var fit_data_subset simulate_index option.csv log.csv age_avg.csv variable.csv var_id var_type s_id m_id time rate integrand covariate node fixed depend start scale fit_value truth sam_avg sam_std res_value res_dage res_dtime lag_value lag_dage lag_dtime sim_v sim_a sim_t prior_info data.csv data_id data_extra_columns child weight age_lo age_up time_lo time_up out density eta nu meas_std meas_stdcv meas_delta meas_value avgint residual sim_value sim_stdcv sim_delta covariates predict.csv avgint_id avgint_extra_columns s_index example ihme_db.sh ',
'db2csv_command.py  init Command: Example and Test  ',' ',
'ihme_db.sh  Make a Local Copy an IHME Dismod_at Database  ',' syntax purpose download python3 absolute_dir --user relative_dir db2csv ',
'get_name_type  Get Column Names and Types in a Table  ',' syntax connection table_name col_name col_type primary key example ',
'get_name_type.py  get_name_type: Example and Test  ',' ',
'get_row_list  Get Data From a Table  ',' syntax connection table_name col_name example ',
'get_row_list.py  get_row_list: Example and Test  ',' ',
'get_table_dict  Get All Data From a Table  ',' syntax connection tbl_name example ',
'get_table_dict.py  get_table_dict: Example and Test  ',' ',
'metropolis  Metropolis MCMC Algorithm  ',' syntax log_f x0 vector case float example ',
'user_metropolis.py  Predict Average Integrand Using Results of a Fit  ',' source code ',
'modify_command  Modify a Column of an Sqlite Database  ',' syntax program python function see also warning arg_list table_name column_name row_expression value_expression expressions statements log example ',
'modify_command.py  init Command: Example and Test  ',' ',
'sql_command  Execute an SQL command  ',' syntax connection result ',
'unicode_tuple  Convert an Iterable Object to a Unicode String  ',' syntax quote_string none infinity u_tuple example ',
'unicode_tuple.py  unicode_tuple: Example and Test  ',' ',
'whats_new_2019  Changes and Additions to Dismod_at During 2019  ',' 09-20 09-19 09-17 09-15 09-13 09-12 09-11 08-24 07-25 07-24 07-20 07-19 07-13 07-10 07-06 06-28 06-27 06-26 06-25 06-24 06-23 06-22 06-21 06-07 06-02 06-01 05-24 05-21 05-12 05-07 05-05 05-03 05-02 05-01 04-30 04-29 04-28 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-18 04-17 04-15 04-14 04-12 04-11 04-10 04-09 04-08 04-06 02-16 01-26 01-23 01-22 01-16 01-15 01-10 ',
'whats_new_2018  Changes and Additions to Dismod_at During 2018  ',' 12-19 12-15 12-14 10-30 10-22 10-21 10-13 10-05 09-30 09-28 09-24 09-14 08-30 08-29 08-22 08-13 08-09 08-08 08-07 08-06 07-31 07-30 07-27 07-26 api other 07-25 07-23 07-21 07-13 06-30 06-29 06-21 06-20 06-18 06-14 06-09 06-04 05-24 05-23 05-21 05-14 05-09 05-08 05-07 05-05 05-04 05-02 04-30 04-26 04-25 04-24 04-23 04-22 04-21 04-16 04-13 04-12 04-11 04-10 04-09 04-08 04-07 04-06 03-21 03-13 03-10 03-08 03-06 02-20 02-17 02-14 02-13 02-12 02-11 02-09 02-08 02-05 02-04 02-03 02-02 02-01 01-31 01-30 01-28 01-26 01-25 01-24 01-23 01-22 01-21 01-20 01-18 01-16 01-12 01-09 ',
'whats_new_2017  Changes and Additions to Dismod_at During 2017  ',' 12-30 12-22 12-21 12-20 12-16 12-13 12-10 11-12 11-08 10-25 10-24 10-22 10-21 10-20 10-18 10-17 10-14 10-13 10-09 10-06 10-05 10-04 10-02 10-01 09-30 09-29 09-28 09-27 09-26 09-25 09-23 09-21 09-20 09-19 09-18 09-17 09-16 09-02 09-01 08-30 08-07 05-23 05-05 05-02 05-01 04-29 04-27 04-26 04-25 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-16 04-15 04-09 04-07 04-05 04-04 04-03 04-01 03-30 03-29 03-17 03-06 03-04 03-03 01-27 01-26 01-24 01-14 01-15 ',
'whats_new_2016  Changes and Additions to Dismod_at During 2016  ',' 12-03 10-31 10-20 10-19 10-18 10-17 10-16 10-15 10-14 10-13 10-12 10-11 09-30 09-29 09-21 08-28 07-29 07-28 07-15 07-14 07-11 06-13 06-12 06-06 06-05 05-16 05-15 05-11 05-10 05-09 05-08 05-07 05-06 05-05 05-04 05-03 05-01 04-28 04-27 04-24 04-23 04-22 04-21 04-20 04-19 04-18 04-17 04-11 04-09 04-07 03-29 02-26 02-06 01-26 01-16 01-15 01-10 01-06 01-05 ',
'whats_new_2015  Changes and Additions to Dismod_at During 2015  ',' 12-29 12-24 12-15 12-14 12-10 12-09 11-23 11-22 11-21 11-20 11-17 11-16 11-15 11-14 11-12 11-10 11-09 11-07 11-06 11-05 11-04 11-03 11-02 11-01 10-30 10-29 10-28 10-27 10-26 10-25 10-24 10-20 10-19 10-16 10-15 10-14 09-18 09-03 08-31 08-27 08-21 08-18 08-15 08-14 08-13 08-12 08-09 08-08 08-04 08-03 08-02 08-01 07-29 07-28 07-27 07-25 07-24 07-23 07-22 07-21 07-20 07-19 07-17 07-16 07-15 07-14 07-13 07-11 07-10 07-09 07-08 07-07 07-05 07-04 07-03 07-01 06-30 06-29 06-27 06-26 06-25 06-24 06-23 06-22 ',
'wish_list  Dismod_at Wish List  ',' null prior_id asymptotic statistics more random effects censored laplace create_database ode solution prevalence large excess mortality csv2db command covariate multipliers grids value priors difference sample conditionals checkpointing meas_std diagrams lagrange documentation real world example user examples starting tracing optimization windows install '
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
