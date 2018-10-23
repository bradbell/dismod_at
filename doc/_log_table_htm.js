var list_up0 = [
'dismod_at.htm',
'database.htm',
'data_flow.htm',
'log_table.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'log_table.htm#log_id',
'log_table.htm#message_type',
'log_table.htm#message_type.command',
'log_table.htm#message_type.random_seed',
'log_table.htm#message_type.error',
'log_table.htm#message_type.warning',
'log_table.htm#table_name',
'log_table.htm#row_id',
'log_table.htm#unix_time',
'log_table.htm#message'
];
var list_down3 = [
'intro_math.htm',
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
'log_table.htm',
'var_table.htm',
'data_subset_table.htm',
'depend_var_table.htm',
'start_var_table.htm',
'scale_var_table.htm',
'fit_var_table.htm',
'fit_data_subset_table.htm',
'truth_var_table.htm',
'prior_sim_table.htm',
'data_sim_table.htm',
'sample_table.htm',
'predict_table.htm'
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
