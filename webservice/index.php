<?php
/*!
* FishFeeder
* v1.0.0 - 2012-04-13
* Copyright (c) 2012 Otto Salminen; Licensed under GPL Version 2 %>
*/

if($_POST["timer"]!=""){
	$timer = explode(":", $_POST["timer"]);
	
	if($timer[0]<24 && $timer[0]>=0 && $timer[1]<60 && $timer[1] >= 0){
		file_put_contents("timer", (int)($timer[0]*60*60+$timer[1]*60));
	}
}

function displayTimer(){

	$result=file_get_contents("timer");
	if ($result === false)
	{
    		return "--:--";
	}
	else {
		$hours = (int)$result/(60*60);
                $minutes = (int)($result/60)%60;
		return sprintf( '%02d:%02d', $hours, $minutes );
	}
}

$timer = file_get_contents("timer");
$timeNow = (int) date("G")*60*60 + date("i")*60;

if($timer < $timeNow){
	$timer += 24*60*60;
}

?>

<!DOCTYPE html>
<html lang="en">
    <head>
        <meta charset="utf-8" />
        <title>Fish feeder</title>
        <script language="javascript" type="text/javascript" src="flot/jquery.min.js"></script>
        <script language="javascript" type="text/javascript" src="flot/jquery.flot.min.js"></script>
        <script language="javascript" type="text/javascript" src="flot/jquery.flot.pie.min.js"></script>
        <script type="text/javascript">
         $(document).ready(function(){
            $(function () {

                var plotdata = [
                    { data: Number(<?php echo 24*60*60 - ($timer-$timeNow); ?>), color: "#70B7BA"},
                    { data: Number(<?php echo $timer-$timeNow; ?>), color: "#F1EFE4"}
                ];

                $.plot($(("#dfGraphs")), plotdata, 
                {
                    
                    series: {
                        pie: { 
                            show: true,
                            pieStrokeLineWidth: 0,
                            pieStrokeColor: '#3D4C53',
                            showLabel: false
                        }
                    }                            
                });	
            });
 		 });
        </script>
    </head>
    <body style="background-color:#E7E1D4;font-family:sans-serif;font-size:3em;text-align:center;">
    <img src="feed.png" border="0"/>
    <div style="width:100%;padding-bottom:20px;" align="center">
        <div id="dfGraphs" style="width:400px;height:400px;"/>
	</div>
	<br/>
	<form action="?" method="POST" enctype="multipart/form-data">
		<input name="timer" type="time" style="text-align:center;font-size:1em;color:#3D4C53;background-color:#fff;border:solid 1px #70B7BA;"
		value="<?php echo displayTimer(); ?>"
		onfocusout="this.form.submit();"/>
	</form>
    </body>
</html>
