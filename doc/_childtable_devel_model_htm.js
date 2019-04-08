// Child table for section devel_model
document.write('\
<select onchange="devel_model_child(this)">\
<option>devel_model-&gt;</option>\
<option>devel_data_model</option>\
<option>prior_model</option>\
<option>fit_model</option>\
<option>depend</option>\
<option>adj_integrand</option>\
<option>devel_avg_integrand</option>\
<option>devel_avg_noise_effect</option>\
</select>\
');
function devel_model_child(item)
{	var child_list = [
		'devel_data_model.htm',
		'prior_model.htm',
		'fit_model.htm',
		'depend.htm',
		'adj_integrand.htm',
		'devel_avg_integrand.htm',
		'devel_avg_noise_effect.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
