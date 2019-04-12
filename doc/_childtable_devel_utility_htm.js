// Child table for section devel_utility
document.write('\
<select onchange="devel_utility_child(this)">\
<option>devel_utility-&gt;</option>\
<option>a1_double</option>\
<option>avgint_subset</option>\
<option>bilinear_interp</option>\
<option>child_info</option>\
<option>cohort_ode</option>\
<option>data_subset</option>\
<option>eigen_ode2</option>\
<option>error_exit</option>\
<option>fixed_effect</option>\
<option>get_prior_mean</option>\
<option>number_random_const</option>\
<option>age_avg_grid</option>\
<option>pack_info</option>\
<option>pack_prior</option>\
<option>residual_density</option>\
<option>random_number</option>\
<option>random_effect</option>\
<option>grid2line</option>\
<option>split_space</option>\
<option>time_line_vec</option>\
</select>\
');
function devel_utility_child(item)
{	var child_list = [
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
		'age_avg_grid.htm',
		'pack_info.htm',
		'pack_prior.htm',
		'residual_density.htm',
		'random_number.htm',
		'random_effect.htm',
		'grid2line.htm',
		'split_space.htm',
		'time_line_vec.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
