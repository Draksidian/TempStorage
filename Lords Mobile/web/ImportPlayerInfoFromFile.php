<?php
set_time_limit(60 * 30);
	
include("db_connection.php");

$SkipMapgen=0;

$f = fopen("Players14.txt","rt");
if(!$f)
	exit("Could not open file");

//need to filter out players that do not get updated in this process.
//If we scanned other plears near him and he did not get updated there is a chance he moved or got renamed
// we should process "new" players yet unseen, and players that we lost in a special way. There is a chance they simply namechanged or swapped realm
$k_ind = 0;
$x_ind = 1;
$y_ind = 2;

$name_ind = 3;
$guild_ind = 4;

$might_ind = 5;
$kills_ind = 6;

$time_ind = 7;
$HasPrisoners_ind = 8;

$VIP_ind = 9;
$GuildRank_ind = 10;
$PLevel_ind = 11;

$SuccessfulAttacks_ind = 12;
$FailedAttacks_ind = 13;
$SuccessfulDefenses_ind = 14;

$FailedDefenses_ind = 15;
$TroopsKilled_ind = 16;
$TroopsLost_ind = 17;

$TroopsHealed_ind = 18;
$TroopsWounded_ind = 19;
$TurfsDestroyed_ind = 20;

$LastInd = $TurfsDestroyed_ind;

$FiveMinutes = 5 * 60;
while (($line = fgets($f)) !== false) 
	if(strlen($line)>5)
	{
		$line = str_replace("\n","",$line);
		$parts = explode(" \t ",$line);
		
		//to avoid warnings about vector being too small
		for($i=count($parts);$i<$LastInd;$i++)
			$parts[$i]=0;
		
		//remove HTML chars from fonts before saving to DB
		foreach($parts as $key => $val)
		{
			$parts[$key] = htmlspecialchars_decode($parts[$key]);
			$parts[$key] = str_replace("&colon;",':',$parts[$key]);
			$parts[$key] = str_replace("&lowbar;",'_',$parts[$key]);
			$parts[$key] = str_replace("&vert;",'|',$parts[$key]);
			$parts[$key] = str_replace("&ast;",'*',$parts[$key]);
			$parts[$key] = str_replace("&bsol;",'\\',$parts[$key]);
			$parts[$key] = str_replace("&gt;",'>',$parts[$key]);
			$parts[$key] = str_replace("&lt;",'<',$parts[$key]);
			$parts[$key] = str_replace("&skipit;",'',$parts[$key]);
//			$parts[$key] = str_replace(" ",'',$parts[$key]);
//			if( $val != $parts[$key] )				echo "$val == ".$parts[$key]."<br>\n";
		}
//		echo "<br>";
		$LastUpdated = $parts[$time_ind];
		
		$PLevel = mysql_real_escape_string($parts[$PLevel_ind]);
		$VIP = mysql_real_escape_string($parts[$VIP_ind]);
		$GuildRank = mysql_real_escape_string($parts[$GuildRank_ind]);
		$SuccessfulAttacks = mysql_real_escape_string($parts[$SuccessfulAttacks_ind]);
		$FailedAttacks = mysql_real_escape_string($parts[$FailedAttacks_ind]);
		$SuccessfulDefenses = mysql_real_escape_string($parts[$SuccessfulDefenses_ind]);
		$FailedDefenses = mysql_real_escape_string($parts[$FailedDefenses_ind]);
		$TroopsKilled = mysql_real_escape_string($parts[$TroopsKilled_ind]);
		$TroopsLost = mysql_real_escape_string($parts[$TroopsLost_ind]);
		$TroopsHealed = mysql_real_escape_string($parts[$TroopsHealed_ind]);
		$TroopsWounded = mysql_real_escape_string($parts[$TroopsWounded_ind]);
		$TurfsDestroyed = mysql_real_escape_string($parts[$TurfsDestroyed_ind]);
		
		//chek if this location exists in DB and if it's newer than what we know
		$query1 = "select LastUpdated,kills,PLevel,VIP,GuildRank,SuccessfulAttacks,FailedAttacks,SuccessfulDefenses,FailedDefenses,TroopsKilled,TroopsLost,TroopsHealed,TroopsWounded,TurfsDestroyed from players where k ='".$parts[$k_ind]."' and x='".$parts[1]."' and y='".$parts[2]."'";
		$result1 = mysql_query($query1,$dbi) or die("Error : 2017022001 <br> ".$query1." <br> ".mysql_error($dbi));
		list( $LastUpdated2, $kills, $PLevel2,$VIP2,$GuildRank2,$SuccessfulAttacks2,$FailedAttacks2,$SuccessfulDefenses2,$FailedDefenses2,$TroopsKilled2,$TroopsLost2,$TroopsHealed2,$TroopsWounded2,$TurfsDestroyed2 ) = mysql_fetch_row( $result1 );

		//if the value in the DB is newer than the one we provided in the scan, it means it should be skipped and not updated. This can happen when multiple bots are scanning the same map and one goes faster than the other
		if( $LastUpdated2 + $FiveMinutes>= $LastUpdated )
			continue;
		
		if($LastUpdated2>0)
			$ArchiveLocation = 1;
		else
			$ArchiveLocation = 0;	// does not yet exist in DB
		
		//if we did not fetch these data this time, inherit it from last available date
		if($PLevel2>$PLevel)
			$PLevel = $PLevel2;
		if($VIP2>$VIP)
			$VIP = $VIP2;
		if($GuildRank==0)
			$GuildRank = $GuildRank2;
		if($SuccessfulAttacks==0)
			$SuccessfulAttacks = $SuccessfulAttacks2;
		if($FailedAttacks==0)
			$FailedAttacks = $FailedAttacks2;
		if($SuccessfulDefenses==0)
			$SuccessfulDefenses = $SuccessfulDefenses2;
		if($FailedDefenses==0)
			$FailedDefenses = $FailedDefenses2;
		if($TroopsKilled==0)
			$TroopsKilled = $TroopsKilled2;
		if($TroopsLost==0)
			$TroopsLost = $TroopsLost2;
		if($TroopsHealed==0)
			$TroopsHealed = $TroopsHealed2;
		if($TroopsWounded==0)
			$TroopsWounded = $TroopsWounded2;
		if($TurfsDestroyed==0)
			$TurfsDestroyed = $TurfsDestroyed2;
		
		//check if this player already exists in another location. Maybe he teleported to a new location
		$query1 = "select LastUpdated,kills,PLevel,VIP,GuildRank,SuccessfulAttacks,FailedAttacks,SuccessfulDefenses,FailedDefenses,TroopsKilled,TroopsLost,TroopsHealed,TroopsWounded,TurfsDestroyed from players where name like '".mysql_real_escape_string($parts[$name_ind])."' and k ='".$parts[$k_ind]."' and not(x='".$parts[1]."' and y='".$parts[2]."') limit 0,1";
		$result1 = mysql_query($query1,$dbi) or die("Error : 20170220012 <br> ".$query1." <br> ".mysql_error($dbi));
		list( $NameExistsStamp,$kills,$PLevel2,$VIP2,$GuildRank2,$SuccessfulAttacks2,$FailedAttacks2,$SuccessfulDefenses2,$FailedDefenses2,$TroopsKilled2,$TroopsLost2,$TroopsHealed2,$TroopsWounded2,$TurfsDestroyed2 ) = mysql_fetch_row( $result1 );
		// there is a chance that conflict exists between name and location of a player. In time the inexisting player should get automatically removed due to no updates
		if( $NameExistsStamp + $FiveMinutes >= $LastUpdated )
			continue;	//the name in the DB is newer than the one we loaded from the file. We ignore the file
		
		if( $NameExistsStamp > 0 )
			$ArchiveName = 1;
		else
			$ArchiveName = 0; // does not yet exist in DB
		
		//if we did not fetch these data this time, inherit it from last available date
		if($PLevel2>$PLevel)
			$PLevel = $PLevel2;
		if($VIP2>$VIP)
			$VIP = $VIP2;
		if($GuildRank==0)
			$GuildRank = $GuildRank2;
		if($SuccessfulAttacks==0)
			$SuccessfulAttacks = $SuccessfulAttacks2;
		if($FailedAttacks==0)
			$FailedAttacks = $FailedAttacks2;
		if($SuccessfulDefenses==0)
			$SuccessfulDefenses = $SuccessfulDefenses2;
		if($FailedDefenses==0)
			$FailedDefenses = $FailedDefenses2;
		if($TroopsKilled==0)
			$TroopsKilled = $TroopsKilled2;
		if($TroopsLost==0)
			$TroopsLost = $TroopsLost2;
		if($TroopsHealed==0)
			$TroopsHealed = $TroopsHealed2;
		if($TroopsWounded==0)
			$TroopsWounded = $TroopsWounded2;
		if($TurfsDestroyed==0)
			$TurfsDestroyed = $TurfsDestroyed2;
		
		if($ArchiveLocation!=0)
		{
			//move old to archive
			$query1 = "insert into players_archive ( select * from players where k='".$parts[$k_ind]."' and x='".$parts[1]."' and y='".$parts[2]."')";
			$result1 = mysql_query($query1,$dbi) or die("Error : 2017022002 <br>".$query1." <br> ".mysql_error($dbi));			
			//delete it
			$query1 = "delete from players where k='".$parts[$k_ind]."' and x='".$parts[1]."' and y='".$parts[2]."'";
			$result1 = mysql_query($query1,$dbi) or die("Error : 20170220023 <br>".$query1." <br> ".mysql_error($dbi));
		}
		if($ArchiveName!=0)
		{
			//move to archive the one from DB 
			$query1 = "insert into players_archive ( select * from players where name like '".mysql_real_escape_string($parts[$name_ind])."' and k ='".$parts[$k_ind]."' limit 0,1)";
			$result1 = mysql_query($query1,$dbi) or die("Error : 20170220022 <br>".$query1." <br> ".mysql_error($dbi));
			//delete it
			$query1 = "delete from players where name like '".mysql_real_escape_string($parts[$name_ind])."'";
			$result1 = mysql_query($query1,$dbi) or die("Error : 20170220023 <br>".$query1." <br> ".mysql_error($dbi));
		}

		//create new
		$query1 = "insert into players ( k,x,y,name,guild,kills,might,lastupdated,HasPrisoners,VIP,GuildRank,PLevel,SuccessfulAttacks,FailedAttacks,SuccessfulDefenses,FailedDefenses,TroopsKilled,TroopsLost,TroopsHealed,TroopsWounded,TurfsDestroyed)values(";
		$query1 .= "'".mysql_real_escape_string($parts[$k_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$x_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$y_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$name_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$guild_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$might_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$kills_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$time_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($parts[$HasPrisoners_ind])."'";
		$query1 .= ",'".mysql_real_escape_string($VIP)."'";
		$query1 .= ",'".mysql_real_escape_string($GuildRank)."'";
		$query1 .= ",'".mysql_real_escape_string($PLevel)."'";
		$query1 .= ",'".mysql_real_escape_string($SuccessfulAttacks)."'";
		$query1 .= ",'".mysql_real_escape_string($FailedAttacks)."'";
		$query1 .= ",'".mysql_real_escape_string($SuccessfulDefenses)."'";
		$query1 .= ",'".mysql_real_escape_string($FailedDefenses)."'";
		$query1 .= ",'".mysql_real_escape_string($TroopsKilled)."'";
		$query1 .= ",'".mysql_real_escape_string($TroopsLost)."'";
		$query1 .= ",'".mysql_real_escape_string($TroopsHealed)."'";
		$query1 .= ",'".mysql_real_escape_string($TroopsWounded)."'";
		$query1 .= ",'".mysql_real_escape_string($TurfsDestroyed)."'";
		$query1 .= ")";

//			echo "$query1<br>";
		$result1 = mysql_query($query1,$dbi) or die("Error : 2017022004 <br>".$query1." <br> ".mysql_error($dbi));
		
		//delete old ones in the neighbourhood
		$xmin = $parts[$x_ind] - 10;
		$xmax = $parts[$x_ind] + 10;
		$ymin = $parts[$y_ind] - 10;
		$ymax = $parts[$y_ind] + 10;
		$olderthan = $parts[$time_ind] - 24 * 60 * 60;
		//move to archive 
		$query1 = "insert into players_archive ( select * from players where x < $xmax and x > $xmin and y < $ymax and y < $ymin and LastUpdated < $olderthan)";
		$result1 = mysql_query($query1,$dbi) or die("Error : 20170220022 <br>".$query1." <br> ".mysql_error($dbi));
		//delete from actual
		$query1 = "delete from players where x < $xmax and x > $xmin and y < $ymax and y < $ymin and LastUpdated < $olderthan";
		$result1 = mysql_query($query1,$dbi) or die("Error : 20170220023 <br>".$query1." <br> ".mysql_error($dbi));
	}
/**/	

if($SkipMapgen)
	exit("Skipped mapgen as requested");

//update innactivity column
$query1 = "update players set innactive=0";
$result1 = mysql_query($query1,$dbi) or die("Error : 20170220027 <br>".$query1." <br> ".mysql_error($dbi));
// a player is innactive if he did not change coordinate and he's might did not change in the past X days
$query1 = "select k,x,y,name,might,lastupdated from players";
$result1 = mysql_query($query1,$dbi) or die("Error : 20170220024 <br>".$query1." <br> ".mysql_error($dbi));
while( list( $k,$x,$y,$name,$might,$lastupdated ) = mysql_fetch_row( $result1 ))
{
	// check he's might yesterday or anywhere before last seen him here. 
	// Kill count might go up when he is defending because of traps
	// should cgeck reource mined
	// should check if he had prisoners recently
	// even if might does not change. Troops healed or troops trained might have changed
	$query2 = "select might,lastupdated from players_archive where k=$k and x=$x and y=$y and lastupdated<".($lastupdated-60*60*24*1)." and name like '".mysql_real_escape_string($name)."' limit 0,1";
//echo "$query2<br>";
	$result2 = mysql_query($query2,$dbi) or die("Error : 20170220025 <br>".$query2." <br> ".mysql_error($dbi));
	$MightChanged = -1;
	$SameMightSince = 1;
	while( list( $MightOld,$lastupdated ) = mysql_fetch_row( $result2 ) )
	{
		if( $MightOld != $might )
		{
			$MightChanged = 1;
			break;
		}
		else
		{
			if(	$SameMightSince > $lastupdated )
				$SameMightSince = $lastupdated;
			if( $MightChanged == -1 )
				$MightChanged = 0;
		}
//echo "Found archive for player $name<br>";
	}
	if($MightChanged==0)
	{
		$query2 = "update players set Innactive=$SameMightSince where k=$k and x=$x and y=$y";
//echo "$query2<br>";
		$result2 = mysql_query($query2,$dbi) or die("Error : 20170220028 <br>".$query2." <br> ".mysql_error($dbi));
//		echo "Player's $name might did not change<br>";
	}
}
//generate new hives
include("gen_hives.php");

//generate static minimaps
$k=67;
$TrackWhat = "might";
include("map_generic.php");
$TrackWhat = "kills";
include("map_generic.php");
$TrackWhat = "pcount";
include("map_generic.php");
$TrackWhat = "guildless";
include("map_generic.php");
$TrackWhat = "guildless_innactive";
include("map_generic.php");
?>