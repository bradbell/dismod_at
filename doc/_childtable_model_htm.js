// Child table for section model
document.write('\
<select onchange="model_child(this)">\
<option>model-&gt;</option>\
<option>model_variables</option>\
<option>avg_integrand</option>\
<option>data_like</option>\
<option>variable_prior</option>\
<option>statistic</option>\
<option>bilinear</option>\
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
		'variable_prior.htm',
		'statistic.htm',
		'bilinear.htm',
		'posterior.htm',
		'prev_dep.htm',
		'numeric_average.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
