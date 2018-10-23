var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_model.htm',
'devel_avg_integrand.htm',
'avg_integrand_add_cohort.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'avg_integrand_add_cohort.htm#Syntax',
'avg_integrand_add_cohort.htm#Prototype',
'avg_integrand_add_cohort.htm#time_ini',
'avg_integrand_add_cohort.htm#time_lower',
'avg_integrand_add_cohort.htm#time_upper',
'avg_integrand_add_cohort.htm#weight_id',
'avg_integrand_add_cohort.htm#integrand_id',
'avg_integrand_add_cohort.htm#n_child_',
'avg_integrand_add_cohort.htm#child',
'avg_integrand_add_cohort.htm#x',
'avg_integrand_add_cohort.htm#x.Float',
'avg_integrand_add_cohort.htm#pack_vec',
'avg_integrand_add_cohort.htm#time_line_object',
'avg_integrand_add_cohort.htm#line_adj',
'avg_integrand_add_cohort.htm#Member Variables',
'avg_integrand_add_cohort.htm#Member Variables.line_age_',
'avg_integrand_add_cohort.htm#Member Variables.line_time_'
];
var list_down4 = [
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
var list_down3 = [
'devel_table.htm',
'devel_utility.htm',
'devel_model.htm'
];
var list_down2 = [
'devel_data_model.htm',
'prior_model.htm',
'fit_model.htm',
'depend.htm',
'adj_integrand.htm',
'devel_avg_integrand.htm',
'devel_avg_std_effect.htm'
];
var list_down1 = [
'avg_integrand_ctor.htm',
'avg_integrand_rectangle.htm',
'avg_integrand_add_cohort.htm'
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
