// Child table for section fit_model_get_solution
document.write('\
<select onchange="fit_model_get_solution_child(this)">\
<option>fit_model_get_solution-&gt;</option>\
<option>fit_model_xam.cpp</option>\
</select>\
');
function fit_model_get_solution_child(item)
{	var child_list = [
		'fit_model_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
