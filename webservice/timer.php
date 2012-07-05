<?php
/*!
* FishFeeder
* v1.0.0 - 2012-04-13
* Copyright (c) 2012 Otto Salminen; Licensed under GPL Version 2 %>
*/
echo ((int) date("G")*60*60 + date("i")*60)."\n";
echo file_get_contents("timer");
