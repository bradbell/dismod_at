// Child table for section prior_fixed_effect
document.write('\
<select onchange="prior_fixed_effect_child(this)">\
<option>prior_fixed_effect-&gt;</option>\
<option>prior_fixed_xam.cpp</option>\
</select>\
');
function prior_fixed_effect_child(item)
{	var child_list = [
		'prior_fixed_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
