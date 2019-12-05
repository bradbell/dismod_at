// Child table for section input
document.write('\
<select onchange="input_child(this)">\
<option>input-&gt;</option>\
<option>age_table</option>\
<option>avgint_table</option>\
<option>covariate_table</option>\
<option>data_table</option>\
<option>density_table</option>\
<option>integrand_table</option>\
<option>mulcov_table</option>\
<option>node_table</option>\
<option>nslist_pair_table</option>\
<option>nslist_table</option>\
<option>option_table</option>\
<option>prior_table</option>\
<option>rate_table</option>\
<option>smooth_grid_table</option>\
<option>smooth_table</option>\
<option>subgroup_table</option>\
<option>time_table</option>\
<option>weight_grid_table</option>\
<option>weight_table</option>\
</select>\
');
function input_child(item)
{	var child_list = [
		'age_table.htm',
		'avgint_table.htm',
		'covariate_table.htm',
		'data_table.htm',
		'density_table.htm',
		'integrand_table.htm',
		'mulcov_table.htm',
		'node_table.htm',
		'nslist_pair_table.htm',
		'nslist_table.htm',
		'option_table.htm',
		'prior_table.htm',
		'rate_table.htm',
		'smooth_grid_table.htm',
		'smooth_table.htm',
		'subgroup_table.htm',
		'time_table.htm',
		'weight_grid_table.htm',
		'weight_table.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
