<?php
if(!isset($dbi))
	include("db_connection.php");

$MaxX = 510;
$MaxY = 1030;
		
if(!isset($k))
	$k = 67;
				
$MinPCount = 10;
$RadiusHalf = 10;
				
$query2 = "delete from guild_hives_multi";
$result2 = mysql_query($query2,$dbi) or die("Error : 20170220042 <br>".$query2." <br> ".mysql_error($dbi));
				
// hive is a location where at least 20 players are grouped in a less than 20x20 square from the same guild		
$MaxMight = 0;
for( $y=$RadiusHalf;$y<$MaxY-$RadiusHalf;$y+=$RadiusHalf*2)
	for( $x=$RadiusHalf;$x<$MaxX-$RadiusHalf;$x += $RadiusHalf*2)
	{
		unset($UniqueGuilds);
		unset($PlayerLocations);
		$query1 = "select guildfull,x,y from players where x>=".($x-$RadiusHalf)." and x<=".($x+$RadiusHalf)." and y>=".($y-$RadiusHalf)." and y<=".($y+$RadiusHalf)."";		
		$result1 = mysql_query($query1,$dbi) or die("Error : 20170220041 <br>".$query1." <br> ".mysql_error($dbi));
		while( list( $guild1,$x1,$y1 ) = mysql_fetch_row( $result1 ))
		{
			if(!isset($PlayerLocations[$guild1]['x']))
				$PlayerLocations[$guild1]['x']=0;
			if(!isset($PlayerLocations[$guild1]['y']))
				$PlayerLocations[$guild1]['y']=0;
			if(!isset($PlayerLocations[$guild1]['c']))
				$PlayerLocations[$guild1]['c']=0;
			$PlayerLocations[$guild1]['x'] += $x1;
			$PlayerLocations[$guild1]['y'] += $y1;
			$PlayerLocations[$guild1]['c']++;
			
			if(isset($UniqueGuilds[$guild1]))
				$UniqueGuilds[$guild1]++;
			else
				$UniqueGuilds[$guild1]=0;
		}
		
		//do we have enough players ?
		if(isset($UniqueGuilds))
		foreach($UniqueGuilds as $name => $count)
//			if($count>$MinPCount)
			{
				$avg_x = (int)($PlayerLocations[$name]['x'] / $PlayerLocations[$name]['c']);
				$avg_y = (int)($PlayerLocations[$name]['y'] / $PlayerLocations[$name]['c']);
//				$avg_x = $x;
//				$avg_y = $y;
//				if($name=="None")
//					$name="";
				$query2 = "select sum(might),sum(kills),sum(castlelevel),count(might),guild from players where x>=".($avg_x-$RadiusHalf)." and x<=".($avg_x+$RadiusHalf)." and y>=".($avg_y-$RadiusHalf)." and y<=".($avg_y+$RadiusHalf)." and ";
				if($name=="")
					$query2 .= " ( isnull(GuildFull) or GuildFull like '')";
				else
					$query2 .= " GuildFull like '".mysql_real_escape_string($name)."'";
				$result2 = mysql_query($query2,$dbi) or die("Error : 20170220042 <br>".$query2." <br> ".mysql_error($dbi));
//echo "$query2<br>";
				list( $might,$kill,$castlelevel,$pcount,$guildsmall ) = mysql_fetch_row( $result2 );
				if($pcount>=$MinPCount)
				{
					$avg_clevel = (int)($castlelevel / $pcount);
//					echo "could be a minihive for $name at $avg_x $avg_y with $pcount players, $might, $kill<br>";
					$query2 = "insert into guild_hives_multi (x,y,guild,guildfull,might,kills,clevel,pcount)values($avg_x,$avg_y,'".mysql_real_escape_string($guildsmall)."','".mysql_real_escape_string($name)."',$might,$kill,$avg_clevel,$pcount)";		
					$result2 = mysql_query($query2,$dbi) or die("Error : 20170220042 <br>".$query2." <br> ".mysql_error($dbi));
				}
			}
	}

?>