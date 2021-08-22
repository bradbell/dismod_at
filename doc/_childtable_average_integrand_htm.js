// Child table for section average_integrand
document.write('\
<select onchange="average_integrand_child(this)">\
<option>average_integrand-&gt;</option>\
<option>user_average_integrand.py</option>\
</select>\
');
function average_integrand_child(item)
{	var child_list = [
		'user_average_integrand.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
