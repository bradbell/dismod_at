var list_up0 = [
'dismod_at.htm',
'command.htm',
'set_command.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'set_command.htm#Syntax',
'set_command.htm#database',
'set_command.htm#option',
'set_command.htm#option.name',
'set_command.htm#option.value',
'set_command.htm#option.init',
'set_command.htm#table_out',
'set_command.htm#table_out.start_var',
'set_command.htm#table_out.scale_var',
'set_command.htm#table_out.truth_var',
'set_command.htm#source',
'set_command.htm#source.sample',
'set_command.htm#source.prior_mean',
'set_command.htm#source.fit_var',
'set_command.htm#source.start, scale, truth',
'set_command.htm#Example'
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
'set_command.py.htm'
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
