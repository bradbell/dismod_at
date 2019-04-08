// Child table for section avgint_table
document.write('\
<select onchange="avgint_table_child(this)">\
<option>avgint_table-&gt;</option>\
<option>avgint_table.py</option>\
</select>\
');
function avgint_table_child(item)
{	var child_list = [
		'avgint_table.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
