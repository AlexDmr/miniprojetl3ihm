import * as NF from "../Services/cabinetMedicalService";
import {Component, Input} from "@angular/core";

const style = `
    :host(.dropCandidate) > * {
        background      : lightyellow;
    }
    
    :host(.dropCandidate.canDrop) > * {
        background      : yellow;
    }`;

const htmlTemplate = `
    <section class = "infirmier">
        <img src="data/{{nf.photo}}" />
        <p class="nom">{{nf.nom}}</p>
        <p class="prenom">{{nf.prenom}}</p>
        <p class="id">{{nf.id}}</p>
        <hr/>
        <h3>Liste des patients !!!!</h3>
        <composant-patient *ngFor              = "let patient of nf.patients"
                           [alx-draggable]     = "{type: 'patient', patient: patient, infirmierSource: nf}"
                           [nf]                = "patient" 
                           ></composant-patient>
    </section>`;

@Component({
    selector	: "composant-infirmier",
    styles      : [ style ],
    template	: htmlTemplate
})
export class ComposantInfirmier {
    @Input() nf         : NF.InfirmierInterface;
    constructor		() {
        console.log( "new instance of ComposantInfirmier", this);
    }
};

