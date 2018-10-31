var list_up0 = [
'dismod_at.htm',
'database.htm',
'input.htm',
'option_table.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'option_table.htm#Conventions',
'option_table.htm#Conventions.Changing Values',
'option_table.htm#Conventions.option_id',
'option_table.htm#Conventions.option_name',
'option_table.htm#Conventions.option_value',
'option_table.htm#parent_node_id',
'option_table.htm#parent_node_id.parent_node_name',
'option_table.htm#parent_node_id.Default',
'option_table.htm#parent_node_id.Children',
'option_table.htm#parent_node_id.Data Subset Table',
'option_table.htm#parent_node_id.Avgint Subset Table',
'option_table.htm#parent_node_id.Changing',
'option_table.htm#zero_sum_random',
'option_table.htm#zero_sum_random.Default',
'option_table.htm#data_extra_columns',
'option_table.htm#data_extra_columns.Default',
'option_table.htm#avgint_extra_columns',
'option_table.htm#avgint_extra_columns.Default',
'option_table.htm#warn_on_stderr',
'option_table.htm#warn_on_stderr.Default',
'option_table.htm#minimum_meas_cv',
'option_table.htm#minimum_meas_cv.Default',
'option_table.htm#ode_step_size',
'option_table.htm#ode_step_size.Default',
'option_table.htm#age_avg_split',
'option_table.htm#age_avg_split.Age Average Grid',
'option_table.htm#age_avg_split.Default',
'option_table.htm#random_seed',
'option_table.htm#random_seed.Default',
'option_table.htm#rate_case',
'option_table.htm#rate_case.iota_zero_rho_zero',
'option_table.htm#rate_case.iota_pos_rho_zero',
'option_table.htm#rate_case.iota_zero_rho_pos',
'option_table.htm#rate_case.iota_pos_rho_pos',
'option_table.htm#rate_case.Default',
'option_table.htm#Fixed and Random',
'option_table.htm#Fixed and Random.derivative_test',
'option_table.htm#Fixed and Random.max_num_iter',
'option_table.htm#Fixed and Random.print_level',
'option_table.htm#Fixed and Random.accept_after_max_steps',
'option_table.htm#Fixed and Random.tolerance',
'option_table.htm#Fixed Only',
'option_table.htm#Fixed Only.quasi_fixed',
'option_table.htm#Fixed Only.bound_frac_fixed',
'option_table.htm#Fixed Only.limited_memory_max_history_fixed',
'option_table.htm#Random Only',
'option_table.htm#Random Only.bound_random',
'option_table.htm#Example'
];
var list_down3 = [
'math_abstract.htm',
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
var list_down2 = [
'input.htm',
'data_flow.htm'
];
var list_down1 = [
'age_table.htm',
'avgint_table.htm',
'covariate_table.htm',
'data_table.htm',
'density_table.htm',
'integrand_table.htm',
'mulcov_table.htm',
'node_table.htm',
'nslist_pair_table.htm',
'nslist_table.htm',
'option_table.htm',
'prior_table.htm',
'rate_table.htm',
'smooth_grid_table.htm',
'smooth_table.htm',
'time_table.htm',
'weight_grid_table.htm',
'weight_table.htm'
];
var list_down0 = [
'option_table.py.htm'
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
function choose_down3(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down3[index-1];
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
