var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_utility.htm',
'data_subset.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'data_subset.htm#Syntax',
'data_subset.htm#See Also',
'data_subset.htm#Purpose',
'data_subset.htm#data_table',
'data_subset.htm#data_cov_value',
'data_subset.htm#covariate_table',
'data_subset.htm#child_object',
'data_subset.htm#data_subset_obj',
'data_subset.htm#data_subset_obj.n_subset',
'data_subset.htm#data_subset_obj.subset_id',
'data_subset.htm#data_subset_obj.original_id',
'data_subset.htm#data_subset_cov_value',
'data_subset.htm#Contents',
'data_subset.htm#Example'
];
var list_down3 = [
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
var list_down2 = [
'devel_table.htm',
'devel_utility.htm',
'devel_model.htm'
];
var list_down1 = [
'a1_double.htm',
'avgint_subset.htm',
'bilinear_interp.htm',
'child_info.htm',
'cohort_ode.htm',
'data_subset.htm',
'eigen_ode2.htm',
'error_exit.htm',
'fixed_effect.htm',
'get_prior_mean.htm',
'number_random_const.htm',
'avg_age_grid.htm',
'pack_info.htm',
'pack_prior.htm',
'residual_density.htm',
'random_number.htm',
'random_effect.htm',
'grid2line.htm',
'solve_ode.htm',
'split_space.htm',
'time_line_vec.htm'
];
var list_down0 = [
'data_subset_xam.cpp.htm'
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
