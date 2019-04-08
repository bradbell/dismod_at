// Child table for section solve_ode
document.write('\
<select onchange="solve_ode_child(this)">\
<option>solve_ode-&gt;</option>\
<option>solve_ode_xam.cpp</option>\
</select>\
');
function solve_ode_child(item)
{	var child_list = [
		'solve_ode_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
