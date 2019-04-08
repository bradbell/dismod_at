// Child table for section avg_noise_effect_rectangle
document.write('\
<select onchange="avg_noise_effect_rectangle_child(this)">\
<option>avg_noise_effect_rectangle-&gt;</option>\
<option>avg_noise_effect_xam.cpp</option>\
</select>\
');
function avg_noise_effect_rectangle_child(item)
{	var child_list = [
		'avg_noise_effect_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
