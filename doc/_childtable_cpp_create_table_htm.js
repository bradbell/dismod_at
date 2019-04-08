// Child table for section cpp_create_table
document.write('\
<select onchange="cpp_create_table_child(this)">\
<option>cpp_create_table-&gt;</option>\
<option>create_table_xam.cpp</option>\
</select>\
');
function cpp_create_table_child(item)
{	var child_list = [
		'create_table_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
