// Child table for section get_row_list
document.write('\
<select onchange="get_row_list_child(this)">\
<option>get_row_list-&gt;</option>\
<option>get_row_list.py</option>\
</select>\
');
function get_row_list_child(item)
{	var child_list = [
		'get_row_list.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
