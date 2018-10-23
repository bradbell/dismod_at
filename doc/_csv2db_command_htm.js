var list_up0 = [
'dismod_at.htm',
'python.htm',
'csv2db_command.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'csv2db_command.htm#Syntax',
'csv2db_command.htm#Syntax.As Program',
'csv2db_command.htm#Syntax.As Python Function',
'csv2db_command.htm#See Also',
'csv2db_command.htm#Example',
'csv2db_command.htm#mtall',
'csv2db_command.htm#mtother',
'csv2db_command.htm#mtother.Constraint',
'csv2db_command.htm#mtother.Age-Time Pairs',
'csv2db_command.htm#mtother.Rectangular Grid',
'csv2db_command.htm#mtother.hold_out',
'csv2db_command.htm#mtother.Rate Grid',
'csv2db_command.htm#Predictions',
'csv2db_command.htm#database',
'csv2db_command.htm#configure_csv',
'csv2db_command.htm#configure_csv.non_zero_rates',
'csv2db_command.htm#measure_csv',
'csv2db_command.htm#integrand',
'csv2db_command.htm#integrand.Sincidence',
'csv2db_command.htm#integrand.remission',
'csv2db_command.htm#integrand.mtexcess',
'csv2db_command.htm#integrand.mtother',
'csv2db_command.htm#integrand.mtwith',
'csv2db_command.htm#integrand.susceptible',
'csv2db_command.htm#integrand.withC',
'csv2db_command.htm#integrand.prevalence',
'csv2db_command.htm#integrand.Tincidence',
'csv2db_command.htm#integrand.mtspecific',
'csv2db_command.htm#integrand.mtall',
'csv2db_command.htm#integrand.mtstandard',
'csv2db_command.htm#integrand.relrisk',
'csv2db_command.htm#age_lower',
'csv2db_command.htm#age_upper',
'csv2db_command.htm#time_lower',
'csv2db_command.htm#time_upper',
'csv2db_command.htm#meas_value',
'csv2db_command.htm#meas_std',
'csv2db_command.htm#hold_out'
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
