// Child table for section data_flow
document.write('\
<select onchange="data_flow_child(this)">\
<option>data_flow-&gt;</option>\
<option>log_table</option>\
<option>var_table</option>\
<option>data_subset_table</option>\
<option>depend_var_table</option>\
<option>start_var_table</option>\
<option>scale_var_table</option>\
<option>age_avg_table</option>\
<option>fit_var_table</option>\
<option>fit_data_subset_table</option>\
<option>truth_var_table</option>\
<option>prior_sim_table</option>\
<option>data_sim_table</option>\
<option>sample_table</option>\
<option>hes_fixed_table</option>\
<option>hes_random_table</option>\
<option>predict_table</option>\
<option>trace_fixed_table</option>\
<option>mixed_info_table</option>\
</select>\
');
function data_flow_child(item)
{	var child_list = [
		'log_table.htm',
		'var_table.htm',
		'data_subset_table.htm',
		'depend_var_table.htm',
		'start_var_table.htm',
		'scale_var_table.htm',
		'age_avg_table.htm',
		'fit_var_table.htm',
		'fit_data_subset_table.htm',
		'truth_var_table.htm',
		'prior_sim_table.htm',
		'data_sim_table.htm',
		'sample_table.htm',
		'hes_fixed_table.htm',
		'hes_random_table.htm',
		'predict_table.htm',
		'trace_fixed_table.htm',
		'mixed_info_table.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
