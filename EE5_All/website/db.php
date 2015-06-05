<?php 

//Connect to the database
$url = "localhost";
$user = "ee5";
$pass = "a*nGC]YE?UC{";
$db = "ee5";
mysql_connect($url, $user, $pass);
mysql_select_db($db);

//Print out the properties for a morris.js line graph
function getGraphJs($dataTable, $dataColumn, $topicNameArray){
	
	//Build data array
	$data = array();
	while($topicName = current($topicNameArray)){
		$q = mysql_query("SELECT timestamp AS 't', {$dataColumn}/10 AS '{$topicName}' FROM {$dataTable} WHERE topicName = '{$topicName}'") or die(mysql_error());
		while($r = mysql_fetch_assoc($q)){
			if($array_match_pos = array_search_recursive("{$r[t]}",$data)){
				//If the timestamp is already present, add the sensor data to the existing sub-array within the data-array
				$data[$array_match_pos]["{$topicName}"] = $r["s{$topicName}"];
			} else{
				//Else, add a new sub-array to the data-array
				array_push($data, $r);
			}
		}
		next($topicNameArray);
	}
	
	//Print out properties
	$json = json_encode($data);
	echo "data:{$json},";
	echo "xkey:'t',";
	echo "ykeys:['".implode("','",$topicNameArray)."'],";
	echo "labels:['Topic ".implode("','Topic ",$topicNameArray)."']";

}

//Recursive array search. Returns the index of the array that contains the array in which the needle is found. Returns false if no matches are found.
function array_search_recursive($needle, $haystack){
	$i = 0;
	while($current_array = current($haystack)){
		if(array_search($needle, $current_array)){
			return $i;
		} else{
			$i++;
			next($haystack);
		}
	}
	return false;
}

?>