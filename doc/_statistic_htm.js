var list_up0 = [
'dismod_at.htm',
'model.htm',
'statistic.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'statistic.htm#Notation',
'statistic.htm#Log-Transformed Standard Deviation, sigma',
'statistic.htm#Weighted Residual Function, R',
'statistic.htm#Weighted Residual Function, R.Value',
'statistic.htm#Weighted Residual Function, R.Difference',
'statistic.htm#Weighted Residual Function, R.Uniform',
'statistic.htm#Weighted Residual Function, R.Gaussian, Laplace, Student\'s-t',
'statistic.htm#Weighted Residual Function, R.Log-Gaussian, Log-Laplace, Log-Student\'s-t',
'statistic.htm#Log-Density Function, D',
'statistic.htm#Log-Density Function, D.Difference',
'statistic.htm#Log-Density Function, D.Uniform',
'statistic.htm#Log-Density Function, D.Gaussian',
'statistic.htm#Log-Density Function, D.Laplace',
'statistic.htm#Log-Density Function, D.Student\'s-t',
'statistic.htm#Log-Density Function, D.Log-Gaussian',
'statistic.htm#Log-Density Function, D.Log-Laplace',
'statistic.htm#Log-Density Function, D.Log-Student\'s-t'
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
'prev_dep.htm'
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
