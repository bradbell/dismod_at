// Child table for section prior_model
document.write('\
<select onchange="prior_model_child(this)">\
<option>prior_model-&gt;</option>\
<option>prior_model_ctor</option>\
<option>replace_mean</option>\
<option>prior_fixed_effect</option>\
<option>prior_random_effect</option>\
</select>\
');
function prior_model_child(item)
{	var child_list = [
		'prior_model_ctor.htm',
		'replace_mean.htm',
		'prior_fixed_effect.htm',
		'prior_random_effect.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
