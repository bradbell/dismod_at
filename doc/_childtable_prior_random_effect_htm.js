// Child table for section prior_random_effect
document.write('\
<select onchange="prior_random_effect_child(this)">\
<option>prior_random_effect-&gt;</option>\
<option>prior_random_xam.cpp</option>\
</select>\
');
function prior_random_effect_child(item)
{	var child_list = [
		'prior_random_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
