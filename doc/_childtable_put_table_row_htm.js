// Child table for section put_table_row
document.write('\
<select onchange="put_table_row_child(this)">\
<option>put_table_row-&gt;</option>\
<option>put_table_row_xam.cpp</option>\
</select>\
');
function put_table_row_child(item)
{	var child_list = [
		'put_table_row_xam.cpp.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
