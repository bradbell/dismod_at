var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_utility.htm',
'pack_prior.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'pack_prior.htm#Syntax',
'pack_prior.htm#Prototype',
'pack_prior.htm#Notation',
'pack_prior.htm#Notation.var_id',
'pack_prior.htm#Notation.prior_id',
'pack_prior.htm#pack_object',
'pack_prior.htm#s_info_vec',
'pack_prior.htm#var2prior',
'pack_prior.htm#size',
'pack_prior.htm#const_value',
'pack_prior.htm#smooth_id',
'pack_prior.htm#value_prior_id',
'pack_prior.htm#dage_prior_id',
'pack_prior.htm#dtime_prior_id',
'pack_prior.htm#dage_var_id',
'pack_prior.htm#dtime_var_id',
'pack_prior.htm#fixed_effect',
'pack_prior.htm#Example'
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
'pack_prior_xam.cpp.htm'
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
