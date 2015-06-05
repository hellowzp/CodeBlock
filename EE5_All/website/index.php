<?php require_once("db.php"); ?>

<!DOCTYPE html>
<html>
    <head>
        <meta charset="utf-8">
        <meta http-equiv="X-UA-Compatible" content="IE=edge">
        <title></title>
        <meta name="description" content="">
        <meta name="viewport" content="width=device-width, initial-scale=1">

        <!--Place favicon.ico and apple-touch-icon.png in the root directory-->

        <link rel="stylesheet" href="css/normalize.css">
        <link rel="stylesheet" href="css/main.css">
        <link rel="stylesheet" href="bower_components/morris.js/morris.css">
    </head>
    <body>

        <!-- Application content starts here -->
        <div class="wrap"><h1><span class="highlight">Levocon</span> dashboard</h1></div>
        
        <div class="box"><div class="wrap">
            <h2>Super neat temperature graph</h2>
            <div id="graph_temperature"></div>
        </div></div>
        
        <div class="box"><div class="wrap">
            <h2>More graphs coming soon!</h2>
            <p>Coming soon right here: graphs in all sizes and flavors.</p>
        </div></div>
		
		<!--Import JS dependencies (jQuery, Raphael and Morris.js)-->
        <script src="//ajax.googleapis.com/ajax/libs/jquery/1.10.2/jquery.min.js"></script>
        <script src="bower_components/raphael/raphael-min.js"></script>
        <script src="bower_components/morris.js/morris.min.js"></script>
        
        <code>
        	<?php //getGraphJs("log_temperature","temperature",array("controller","wasp/temp")); ?>
        </code>
		
		<!--Get some graphs up in here!-->
		<script>
			$(document).ready(function(){
				Morris.Line({
				  element: "graph_temperature",
				  <?php getGraphJs("log_temperature","temperature",array("controller","wasp/temp")); ?>
				});
			});
		</script>
		
        <!--Google Analytics: change UA-XXXXX-X to be your site's ID.-->
        <!--<script>
            (function(b,o,i,l,e,r){b.GoogleAnalyticsObject=l;b[l]||(b[l]=
            function(){(b[l].q=b[l].q||[]).push(arguments)});b[l].l=+new Date;
            e=o.createElement(i);r=o.getElementsByTagName(i)[0];
            e.src='//www.google-analytics.com/analytics.js';
            r.parentNode.insertBefore(e,r)}(window,document,'script','ga'));
            ga('create','UA-XXXXX-X');ga('send','pageview');
        </script>-->
    </body>
</html>
