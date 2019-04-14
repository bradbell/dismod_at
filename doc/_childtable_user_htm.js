// Child table for section user
document.write('\
<select onchange="user_child(this)">\
<option>user-&gt;</option>\
<option>user_csv2db.py</option>\
<option>user_fit_random.py</option>\
<option>user_fit_fixed_both.py</option>\
<option>user_posterior.py</option>\
<option>user_fit_sim.py</option>\
<option>user_fit_gamma.py</option>\
<option>age_avg_split.py</option>\
<option>user_diabetes.py</option>\
<option>user_ill_condition.py</option>\
<option>user_jump_at_age.py</option>\
<option>user_change_grid.py</option>\
<option>user_lasso_covariate.py</option>\
<option>user_asymptotic.py</option>\
<option>user_const_random.py</option>\
<option>user_const_value.py</option>\
<option>user_continue_fit.py</option>\
<option>user_diff_constraint.py</option>\
<option>user_meas_covariate.py</option>\
<option>user_no_children.py</option>\
<option>user_predict_fit.py</option>\
<option>user_zero_sum_random.py</option>\
<option>user_re_scale.py</option>\
<option>user_speed.py</option>\
<option>user_students.py</option>\
</select>\
');
function user_child(item)
{	var child_list = [
		'user_csv2db.py.htm',
		'user_fit_random.py.htm',
		'user_fit_fixed_both.py.htm',
		'user_posterior.py.htm',
		'user_fit_sim.py.htm',
		'user_fit_gamma.py.htm',
		'age_avg_split.py.htm',
		'user_diabetes.py.htm',
		'user_ill_condition.py.htm',
		'user_jump_at_age.py.htm',
		'user_change_grid.py.htm',
		'user_lasso_covariate.py.htm',
		'user_asymptotic.py.htm',
		'user_const_random.py.htm',
		'user_const_value.py.htm',
		'user_continue_fit.py.htm',
		'user_diff_constraint.py.htm',
		'user_meas_covariate.py.htm',
		'user_no_children.py.htm',
		'user_predict_fit.py.htm',
		'user_zero_sum_random.py.htm',
		'user_re_scale.py.htm',
		'user_speed.py.htm',
		'user_students.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
