var list_up0 = [
'dismod_at.htm',
'model.htm',
'data_like.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'data_like.htm#Data Table Notation',
'data_like.htm#Data Table Notation.i',
'data_like.htm#Data Table Notation.y_i',
'data_like.htm#Data Table Notation.a_i',
'data_like.htm#Data Table Notation.b_i',
'data_like.htm#Data Table Notation.s_i',
'data_like.htm#Data Table Notation.t_i',
'data_like.htm#Data Table Notation.d_i',
'data_like.htm#Data Table Notation.I_i',
'data_like.htm#Data Table Notation.x_ij',
'data_like.htm#Data Table Notation.w_i',
'data_like.htm#Data Table Notation.Delta_i',
'data_like.htm#Measurement Standard Deviation Covariates',
'data_like.htm#Measurement Standard Deviation Covariates.K_i',
'data_like.htm#Measurement Standard Deviation Covariates.gamma_j',
'data_like.htm#Effects, u, theta',
'data_like.htm#Average Integrand',
'data_like.htm#Average Standard Deviation Effect',
'data_like.htm#Adjusted Standard Deviation, delta_i',
'data_like.htm#Weighted Residual and Likelihood'
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
