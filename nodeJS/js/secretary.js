var ControlPanel = require("./ControlPanelInteraction.js").ControlPanel
  , domReady     = require("./domReady");

domReady( function() {
			 console.log(ControlPanel);
			 ControlPanel.init( 'map', 'Affectations', 'newPatientMap' );
			}
		);
