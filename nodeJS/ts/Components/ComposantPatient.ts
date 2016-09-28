import * as NF from "../Services/cabinetMedicalService";
import {Component, Input} from "@angular/core";

@Component({
    selector	: "composant-patient",
    styles      : [
        `section.patient {display: block; border: solid black 1px; background: lightgreen;}`
    ],
    template	: `<section class="patient">
                        <p class="nom">{{nf.nom}}</p>
                        <p class="prenom">{{nf.prenom}}</p>
                        <p class="sexe">{{nf.sexe}}</p>
                        <p class="sexe">{{nf.numeroSecuriteSociale}}</p>
                        <p>{{nf.adresse | json}}</p>
				   </section>`,
    directives	: [],
    providers	: []
})
export class ComposantPatient {
    @Input() nf : NF.PatientInterface;
    constructor		() {
        // XXX
    }
};

