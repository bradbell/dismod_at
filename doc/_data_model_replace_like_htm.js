var list_up0 = [
'dismod_at.htm',
'devel.htm',
'devel_model.htm',
'devel_data_model.htm',
'data_model_replace_like.htm'
];
var list_across0 = [
'_contents.htm',
'_reference.htm',
'_index.htm',
'_search.htm',
'_external.htm'
];
var list_current0 = [
'data_model_replace_like.htm#Syntax',
'data_model_replace_like.htm#Purpose',
'data_model_replace_like.htm#data_object',
'data_model_replace_like.htm#data_object.subset_object',
'data_model_replace_like.htm#data_object.n_subset',
'data_model_replace_like.htm#data_object.subset_id',
'data_model_replace_like.htm#data_subset_obj',
'data_model_replace_like.htm#Laplace Density'
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
'data_model_ctor.htm',
'data_model_replace_like.htm',
'data_model_average.htm',
'data_model_like_one.htm',
'data_model_like_all.htm'
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
