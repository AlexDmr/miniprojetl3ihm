import { NgModule }                 from "@angular/core";
import { CommonModule }             from "@angular/common";
import { DragDropModule }           from "./DragDrop/DragDropModule";
import { ComposantInfirmier }       from "./Components/ComposantInfirmier";
import { ComposantPatient }         from "./Components/ComposantPatient";
import { ComposantSecretaire }      from "./Components/ComposantSecretaire";
import { HttpModule }               from "@angular/http";
import { ServiceCabinetMedical }    from "@Services/cabinetMedicalService";

@NgModule({
    imports     : [ CommonModule, DragDropModule, HttpModule ],
    exports     : [ ComposantSecretaire ],
    declarations: [ ComposantInfirmier, ComposantPatient, ComposantSecretaire ],
    providers   : [ ServiceCabinetMedical ],

})
export class CabinetMedicalModule { }
