// Child table for section model
document.write('\
<select onchange="model_child(this)">\
<option>model-&gt;</option>\
<option>model_variables</option>\
<option>avg_integrand</option>\
<option>data_like</option>\
<option>fixed_value</option>\
<option>fixed_diff</option>\
<option>fixed_prior</option>\
<option>random_value</option>\
<option>random_diff</option>\
<option>random_prior</option>\
<option>statistic</option>\
<option>bilinear</option>\
<option>ode_grid</option>\
<option>posterior</option>\
<option>prev_dep</option>\
<option>numeric_average</option>\
</select>\
');
function model_child(item)
{	var child_list = [
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
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
