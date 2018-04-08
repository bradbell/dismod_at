var list_up0 = [
'dismod_at.htm',
'command.htm',
'fit_command.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'fit_command.htm#Syntax',
'fit_command.htm#database',
'fit_command.htm#variables',
'fit_command.htm#variables.fixed',
'fit_command.htm#variables.random',
'fit_command.htm#variables.both',
'fit_command.htm#simulate_index',
'fit_command.htm#simulate_table',
'fit_command.htm#fit_var_table',
'fit_command.htm#fit_data_subset_table',
'fit_command.htm#Random Effects',
'fit_command.htm#Convergence Problems',
'fit_command.htm#Example'
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
'set_command.htm',
'init_command.htm',
'depend_command.htm',
'fit_command.htm',
'simulate_command.htm',
'sample_command.htm',
'predict_command.htm'
];
var list_down0 = [
'fit_command.py.htm'
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
