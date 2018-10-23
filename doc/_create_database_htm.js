var list_up0 = [
'dismod_at.htm',
'python.htm',
'create_database.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'create_database.htm#Syntax',
'create_database.htm#Purpose',
'create_database.htm#Primary Key',
'create_database.htm#Name Column',
'create_database.htm#file_name',
'create_database.htm#age_list',
'create_database.htm#time_list',
'create_database.htm#integrand_table',
'create_database.htm#node_table',
'create_database.htm#weight_table',
'create_database.htm#covariate_table',
'create_database.htm#avgint_table',
'create_database.htm#avgint_table.avgint_extra_columns',
'create_database.htm#data_table',
'create_database.htm#data_table.data_extra_columns',
'create_database.htm#prior_table',
'create_database.htm#smooth_table',
'create_database.htm#smooth_table.name',
'create_database.htm#smooth_table.age_id',
'create_database.htm#smooth_table.time_id',
'create_database.htm#smooth_table.mulstd_value_prior_name',
'create_database.htm#smooth_table.mulstd_dage_prior_name',
'create_database.htm#smooth_table.mulstd_dtime_prior_name',
'create_database.htm#smooth_table.fun',
'create_database.htm#smooth_table.const_value',
'create_database.htm#nslist_table',
'create_database.htm#rate_table',
'create_database.htm#mulcov_table',
'create_database.htm#option_table',
'create_database.htm#Contents',
'create_database.htm#Example'
];
var list_down2 = [
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
var list_down1 = [
'create_connection.htm',
'create_table.htm',
'create_database.htm',
'csv2db_command.htm',
'db2csv_command.htm',
'get_name_type.htm',
'get_row_list.htm',
'get_table_dict.htm',
'metropolis.htm',
'modify_command.htm',
'sql_command.htm',
'unicode_tuple.htm'
];
var list_down0 = [
'create_database.py.htm'
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
