var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_table.htm',
'get_simulate_table.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'get_simulate_table.htm#Syntax',
'get_simulate_table.htm#Purpose',
'get_simulate_table.htm#db',
'get_simulate_table.htm#simulate_table',
'get_simulate_table.htm#simulate_struct',
'get_simulate_table.htm#Example'
];
var list_down3 = [
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
'devel_table.htm',
'devel_utility.htm',
'devel_model.htm'
];
var list_down1 = [
'check_child_prior.htm',
'check_pini_n_age.htm',
'check_rate_limit.htm',
'check_table_id.htm',
'check_zero_sum.htm',
'cpp_create_table.htm',
'exec_sql_cmd.htm',
'get_age_table.htm',
'get_avgint_table.htm',
'get_column_max.htm',
'get_covariate_table.htm',
'get_data_table.htm',
'get_db_input.htm',
'get_density_table.htm',
'get_integrand_table.htm',
'get_mulcov_table.htm',
'get_node_table.htm',
'get_option_table.htm',
'get_prior_table.htm',
'get_rate_table.htm',
'get_sample_table.htm',
'get_simulate_table.htm',
'get_smooth_grid.htm',
'get_nslist_table.htm',
'get_nslist_pair.htm',
'get_smooth_table.htm',
'get_table_column.htm',
'get_time_table.htm',
'get_weight_grid.htm',
'get_weight_table.htm',
'log_message.htm',
'put_table_row.htm',
'open_connection.htm',
'smooth_info.htm',
'weight_info.htm'
];
var list_down0 = [
'get_simulate_table_xam.cpp.htm'
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