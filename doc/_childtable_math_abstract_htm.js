// Child table for section math_abstract
document.write('\
<select onchange="math_abstract_child(this)">\
<option>math_abstract-&gt;</option>\
<option>math_ode</option>\
<option>censor_density</option>\
<option>prevalence_ode</option>\
</select>\
');
function math_abstract_child(item)
{	var child_list = [
		'math_ode.htm',
		'censor_density.htm',
		'prevalence_ode.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
