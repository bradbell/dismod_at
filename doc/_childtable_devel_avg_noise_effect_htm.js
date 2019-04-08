// Child table for section devel_avg_noise_effect
document.write('\
<select onchange="devel_avg_noise_effect_child(this)">\
<option>devel_avg_noise_effect-&gt;</option>\
<option>avg_noise_effect_ctor</option>\
<option>avg_noise_effect_rectangle</option>\
</select>\
');
function devel_avg_noise_effect_child(item)
{	var child_list = [
		'avg_noise_effect_ctor.htm',
		'avg_noise_effect_rectangle.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
