// Child table for section get_name_type
document.write('\
<select onchange="get_name_type_child(this)">\
<option>get_name_type-&gt;</option>\
<option>get_name_type.py</option>\
</select>\
');
function get_name_type_child(item)
{	var child_list = [
		'get_name_type.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
