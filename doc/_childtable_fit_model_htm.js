// Child table for section fit_model
document.write('\
<select onchange="fit_model_child(this)">\
<option>fit_model-&gt;</option>\
<option>fit_model_ctor</option>\
<option>fit_model_run_fit</option>\
<option>fit_model_get_solution</option>\
<option>fit_model_sample_posterior</option>\
</select>\
');
function fit_model_child(item)
{	var child_list = [
		'fit_model_ctor.htm',
		'fit_model_run_fit.htm',
		'fit_model_get_solution.htm',
		'fit_model_sample_posterior.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
