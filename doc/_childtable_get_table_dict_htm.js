// Child table for section get_table_dict
document.write('\
<select onchange="get_table_dict_child(this)">\
<option>get_table_dict-&gt;</option>\
<option>get_table_dict.py</option>\
</select>\
');
function get_table_dict_child(item)
{	var child_list = [
		'get_table_dict.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
