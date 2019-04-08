// Child table for section avg_integrand_rectangle
document.write('\
<select onchange="avg_integrand_rectangle_child(this)">\
<option>avg_integrand_rectangle-&gt;</option>\
<option>avg_integrand_xam.cpp</option>\
</select>\
');
function avg_integrand_rectangle_child(item)
{	var child_list = [
		'avg_integrand_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
