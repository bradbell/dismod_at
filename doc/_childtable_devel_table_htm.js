// Child table for section devel_table
document.write('\
<select onchange="devel_table_child(this)">\
<option>devel_table-&gt;</option>\
<option>check_child_prior</option>\
<option>check_pini_n_age</option>\
<option>check_rate_limit</option>\
<option>check_table_id</option>\
<option>check_zero_sum</option>\
<option>cpp_create_table</option>\
<option>exec_sql_cmd</option>\
<option>get_age_table</option>\
<option>get_avgint_table</option>\
<option>get_column_max</option>\
<option>get_covariate_table</option>\
<option>get_data_table</option>\
<option>get_db_input</option>\
<option>get_density_table</option>\
<option>get_integrand_table</option>\
<option>get_mulcov_table</option>\
<option>get_node_table</option>\
<option>get_option_table</option>\
<option>get_prior_table</option>\
<option>get_rate_table</option>\
<option>get_sample_table</option>\
<option>get_data_sim_table</option>\
<option>get_smooth_grid</option>\
<option>get_nslist_table</option>\
<option>get_nslist_pair</option>\
<option>get_smooth_table</option>\
<option>get_table_column</option>\
<option>get_time_table</option>\
<option>get_weight_grid</option>\
<option>get_weight_table</option>\
<option>log_message</option>\
<option>put_table_row</option>\
<option>open_connection</option>\
<option>smooth_info</option>\
<option>weight_info</option>\
</select>\
');
function devel_table_child(item)
{	var child_list = [
		'check_child_prior.htm',
		'check_pini_n_age.htm',
		'check_rate_limit.htm',
		'check_table_id.htm',
		'check_zero_sum.htm',
		'cpp_create_table.htm',
		'exec_sql_cmd.htm',
		'get_age_table.htm',
		'get_avgint_table.htm',
		'get_column_max.htm',
		'get_covariate_table.htm',
		'get_data_table.htm',
		'get_db_input.htm',
		'get_density_table.htm',
		'get_integrand_table.htm',
		'get_mulcov_table.htm',
		'get_node_table.htm',
		'get_option_table.htm',
		'get_prior_table.htm',
		'get_rate_table.htm',
		'get_sample_table.htm',
		'get_data_sim_table.htm',
		'get_smooth_grid.htm',
		'get_nslist_table.htm',
		'get_nslist_pair.htm',
		'get_smooth_table.htm',
		'get_table_column.htm',
		'get_time_table.htm',
		'get_weight_grid.htm',
		'get_weight_table.htm',
		'log_message.htm',
		'put_table_row.htm',
		'open_connection.htm',
		'smooth_info.htm',
		'weight_info.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
