var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_up0 = [
'dismod_at.htm',
'database.htm',
'input.htm',
'density_table.htm',
'density_table.py.htm'
];
var list_down4 = [
'install_unix.htm',
'get_started.htm',
'user.htm',
'database.htm',
'model.htm',
'command.htm',
'python.htm',
'devel.htm',
'whats_new_2017.htm',
'wish_list.htm'
];
var list_down3 = [
'input.htm',
'data_flow.htm'
];
var list_down2 = [
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
var list_down1 = [
'density_table.py.htm'
];
function choose_across0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_across0[index-1];
}
function choose_up0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_up0[index-1];
}
function choose_down4(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_down4[index-1];
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
function choose_current0(item)
{	var index          = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = list_current0[index-1];
}