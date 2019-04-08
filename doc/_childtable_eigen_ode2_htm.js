// Child table for section eigen_ode2
document.write('\
<select onchange="eigen_ode2_child(this)">\
<option>eigen_ode2-&gt;</option>\
<option>eigen_ode2_xam.cpp</option>\
</select>\
');
function eigen_ode2_child(item)
{	var child_list = [
		'eigen_ode2_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
