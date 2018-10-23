var list_up0 = [
'dismod_at.htm',
'model.htm',
'numeric_average.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'numeric_average.htm#Average Standard Deviation Effect',
'numeric_average.htm#A',
'numeric_average.htm#ODE',
'numeric_average.htm#Initial Age',
'numeric_average.htm#Rectangle',
'numeric_average.htm#Rectangle.a_L',
'numeric_average.htm#Rectangle.a_U',
'numeric_average.htm#Rectangle.t_L',
'numeric_average.htm#Rectangle.t_U',
'numeric_average.htm#Average Integrand Time Step, Delta t',
'numeric_average.htm#Average Integrand Age Grid',
'numeric_average.htm#Extended Age Grid',
'numeric_average.htm#Cohort Solution of ODE',
'numeric_average.htm#Time Line',
'numeric_average.htm#S_j',
'numeric_average.htm#Lower Time Cohorts',
'numeric_average.htm#Lower Time Cohorts.ODE',
'numeric_average.htm#Upper Time Cohorts',
'numeric_average.htm#Upper Time Cohorts.ODE',
'numeric_average.htm#Refinement',
'numeric_average.htm#Refinement.ODE Not Required',
'numeric_average.htm#Refinement.ODE Required',
'numeric_average.htm#w(a, t)',
'numeric_average.htm#g(a, t)',
'numeric_average.htm#Time Integration',
'numeric_average.htm#Age Integration'
];
var list_down2 = [
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
var list_down1 = [
'model_variables.htm',
'avg_integrand.htm',
'data_like.htm',
'fixed_value.htm',
'fixed_diff.htm',
'fixed_prior.htm',
'random_value.htm',
'random_diff.htm',
'random_prior.htm',
'statistic.htm',
'bilinear.htm',
'ode_grid.htm',
'posterior.htm',
'prev_dep.htm',
'numeric_average.htm'
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
