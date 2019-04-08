// Child table for section get_table_column
document.write('\
<select onchange="get_table_column_child(this)">\
<option>get_table_column-&gt;</option>\
<option>get_table_column_xam.cpp</option>\
</select>\
');
function get_table_column_child(item)
{	var child_list = [
		'get_table_column_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
