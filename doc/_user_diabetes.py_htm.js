var list_up0 = [
'dismod_at.htm',
'user.htm',
'user_diabetes.py.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'user_diabetes.py.htm#Running This example',
'user_diabetes.py.htm#omega',
'user_diabetes.py.htm#rho',
'user_diabetes.py.htm#Weight Table and Grid',
'user_diabetes.py.htm#Covariates',
'user_diabetes.py.htm#Covariates.Covariate Table',
'user_diabetes.py.htm#Covariates.Data Table',
'user_diabetes.py.htm#Covariates.Multipliers',
'user_diabetes.py.htm#Truth Var Table',
'user_diabetes.py.htm#Truth Var Table.Parent Rates',
'user_diabetes.py.htm#Truth Var Table.Child Rates',
'user_diabetes.py.htm#Predict Table',
'user_diabetes.py.htm#Problem Parameters',
'user_diabetes.py.htm#Problem Parameters.mulcov_dict',
'user_diabetes.py.htm#Problem Parameters.node_list',
'user_diabetes.py.htm#Problem Parameters.integrand_list',
'user_diabetes.py.htm#Problem Parameters.parent_age_grid',
'user_diabetes.py.htm#Problem Parameters.child_age_grid',
'user_diabetes.py.htm#Problem Parameters.parent_time_grid',
'user_diabetes.py.htm#Problem Parameters.child_time_grid',
'user_diabetes.py.htm#Problem Parameters.ode_step_size',
'user_diabetes.py.htm#Problem Parameters.meas_cv',
'user_diabetes.py.htm#Problem Parameters.meas_repeat',
'user_diabetes.py.htm#Problem Parameters.fit_with_noise_in_data',
'user_diabetes.py.htm#Problem Parameters.random_seed',
'user_diabetes.py.htm#Problem Parameters.quasi_fixed',
'user_diabetes.py.htm#Problem Parameters.tolerance_fixed',
'user_diabetes.py.htm#Problem Parameters.derivative_test_fixed',
'user_diabetes.py.htm#Problem Parameters.truth2start',
'user_diabetes.py.htm#Problem Parameters.accept_rel_err',
'user_diabetes.py.htm#Source Code'
];
var list_down2 = [
'install_unix.htm',
'get_started.htm',
'user.htm',
'database.htm',
'model.htm',
'command.htm',
'python.htm',
'devel.htm',
'whats_new_2018.htm',
'wish_list.htm'
];
var list_down1 = [
'user_fit_random.py.htm',
'user_asymptotic.py.htm',
'user_const_random.py.htm',
'user_const_value.py.htm',
'user_continue_fit.py.htm',
'user_diabetes.py.htm',
'user_diff_constraint.py.htm',
'user_fit_fixed.py.htm',
'user_fit_sim.py.htm',
'user_jump_at_age.py.htm',
'user_lasso_covariate.py.htm',
'user_meas_covariate.py.htm',
'user_no_children.py.htm',
'user_predict_fit.py.htm',
'user_zero_sum_random.py.htm',
'user_re_scale.py.htm',
'user_speed.py.htm',
'user_students.py.htm'
];
function choose_up0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_up0[index-1];
}
function choose_across0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_across0[index-1];
}
function choose_current0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_current0[index-1];
}
function choose_down2(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down2[index-1];
}
function choose_down1(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down1[index-1];
}
function choose_down0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down0[index-1];
}
