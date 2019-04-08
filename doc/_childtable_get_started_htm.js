// Child table for section get_started
document.write('\
<select onchange="get_started_child(this)">\
<option>get_started-&gt;</option>\
<option>get_started_db.py</option>\
</select>\
');
function get_started_child(item)
{	var child_list = [
		'get_started_db.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
