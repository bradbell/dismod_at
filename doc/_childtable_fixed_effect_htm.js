// Child table for section fixed_effect
document.write('\
<select onchange="fixed_effect_child(this)">\
<option>fixed_effect-&gt;</option>\
<option>fixed_effect_xam.cpp</option>\
</select>\
');
function fixed_effect_child(item)
{	var child_list = [
		'fixed_effect_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
