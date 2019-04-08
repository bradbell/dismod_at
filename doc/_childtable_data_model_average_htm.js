// Child table for section data_model_average
document.write('\
<select onchange="data_model_average_child(this)">\
<option>data_model_average-&gt;</option>\
<option>avg_no_ode_xam.cpp</option>\
<option>avg_yes_ode_xam.cpp</option>\
</select>\
');
function data_model_average_child(item)
{	var child_list = [
		'avg_no_ode_xam.cpp.htm',
		'avg_yes_ode_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
