// Child table for section cohort_ode
document.write('\
<select onchange="cohort_ode_child(this)">\
<option>cohort_ode-&gt;</option>\
<option>cohort_ode_xam.cpp</option>\
</select>\
');
function cohort_ode_child(item)
{	var child_list = [
		'cohort_ode_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
