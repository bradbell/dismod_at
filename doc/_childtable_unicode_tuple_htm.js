// Child table for section unicode_tuple
document.write('\
<select onchange="unicode_tuple_child(this)">\
<option>unicode_tuple-&gt;</option>\
<option>unicode_tuple.py</option>\
</select>\
');
function unicode_tuple_child(item)
{	var child_list = [
		'unicode_tuple.py.htm'
	];
	var index = item.selectedIndex;
	item.selectedIndex = 0;
	if(index > 0)
		document.location = child_list[index-1];
}
