// Child table for section variable_prior
document.write('\
<select onchange="variable_prior_child(this)">\
<option>variable_prior-&gt;</option>\
<option>fixed_value</option>\
<option>random_value</option>\
<option>smooth_dage</option>\
<option>smooth_dtime</option>\
<option>fixed_diff</option>\
<option>random_diff</option>\
<option>fixed_prior</option>\
<option>random_prior</option>\
</select>\
');
function variable_prior_child(item)
{	var child_list = [
		'fixed_value.htm',
		'random_value.htm',
		'smooth_dage.htm',
		'smooth_dtime.htm',
		'fixed_diff.htm',
		'random_diff.htm',
		'fixed_prior.htm',
		'random_prior.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
