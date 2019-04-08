// Child table for section adj_integrand
document.write('\
<select onchange="adj_integrand_child(this)">\
<option>adj_integrand-&gt;</option>\
<option>adj_integrand_xam.cpp</option>\
</select>\
');
function adj_integrand_child(item)
{	var child_list = [
		'adj_integrand_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
