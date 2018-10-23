var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_model.htm',
'adj_integrand.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'adj_integrand.htm#Syntax',
'adj_integrand.htm#Prototype',
'adj_integrand.htm#rate_case',
'adj_integrand.htm#age_table',
'adj_integrand.htm#time_table',
'adj_integrand.htm#integrand_table',
'adj_integrand.htm#s_info_vec',
'adj_integrand.htm#pack_object',
'adj_integrand.htm#line_age',
'adj_integrand.htm#line_age.n_line',
'adj_integrand.htm#line_time',
'adj_integrand.htm#line_time.ODE',
'adj_integrand.htm#integrand_id',
'adj_integrand.htm#n_child_',
'adj_integrand.htm#child',
'adj_integrand.htm#pack_vec',
'adj_integrand.htm#pack_vec.Float',
'adj_integrand.htm#adj_line',
'adj_integrand.htm#Example'
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
'devel_table.htm',
'devel_utility.htm',
'devel_model.htm'
];
var list_down1 = [
'devel_data_model.htm',
'prior_model.htm',
'fit_model.htm',
'depend.htm',
'adj_integrand.htm',
'devel_avg_integrand.htm',
'devel_avg_std_effect.htm'
];
var list_down0 = [
'adj_integrand_xam.cpp.htm'
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
