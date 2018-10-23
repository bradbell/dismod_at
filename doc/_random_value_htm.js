var list_up0 = [
'dismod_at.htm',
'model.htm',
'random_value.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'random_value.htm#c',
'random_value.htm#j',
'random_value.htm#j.k_j',
'random_value.htm#j.a_j',
'random_value.htm#j.t_j',
'random_value.htm#u_c,j',
'random_value.htm#lambda_j',
'random_value.htm#delta_j',
'random_value.htm#d_j',
'random_value.htm#mu_j',
'random_value.htm#V_c,j^u',
'random_value.htm#V^u'
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
