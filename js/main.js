var Tournee = null;
define ( [ "interaction"
		 , 'domReady'
		 ]
	   , function(TourneeInteraction, domReady) {
			 Tournee = TourneeInteraction;
			 domReady( function() {
				 console.log(Tournee);
				 Tournee.init( 'carte', 'chemins' );
				});
			 return Tournee;
			}
);
