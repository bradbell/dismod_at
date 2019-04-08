// Child table for section random_effect
document.write('\
<select onchange="random_effect_child(this)">\
<option>random_effect-&gt;</option>\
<option>random_effect_xam.cpp</option>\
</select>\
');
function random_effect_child(item)
{	var child_list = [
		'random_effect_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
