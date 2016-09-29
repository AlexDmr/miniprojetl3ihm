import * as NF from "@Services/cabinetMedicalService";
import {Component, OnInit} from "@angular/core";

const style = `
    section.infirmiers > .list {
        display: flex;
        flex-flow: row;
        }
     section.infirmiers > .list > * {
        flex : 1 0 auto;
     }`;

const htmlTemplate = `
    <h1 alx-dragdrop>IHM de la secrétaire</h1>
    <p>à compléter...</p>
    <p *ngIf="!initDone">CHARGEMENT...</p>
    <section *ngIf="initDone" class="cabinet">
        <section class="infirmiers">
            <h2>Les infirmier</h2>
            <section class="list">
                <composant-infirmier 
                    *ngFor="let infirmier of nf.infirmiers" 
                    [nf]                 = "infirmier"
                    alx-dropzone 
                    (alx-ondrop)         = "affecterPatient($event.patient, $event.infirmierSource, infirmier)"
                    [alx-accept-fct]     = "isPatient"
                    alx-dragstart-css    = "dropCandidate"
                    alx-draghover-css    = "canDrop"
                    ></composant-infirmier>
            </section>
        </section>
    </section>`;
@Component({
    selector	: "composant-secretaire",
    styles      : [ style ],
    template	: htmlTemplate
})
export class ComposantSecretaire implements OnInit {
    nf              : NF.CabinetInterface;
    initDone        : boolean = false;
    constructor		(public cms: NF.ServiceCabinetMedical) {
        this.nf = cms.cabinetJS;
    }
    affecterPatient(patient: NF.PatientInterface, infirmierOrigine: NF.InfirmierInterface, infirmierDestination: NF.InfirmierInterface) {
        this.cms.affecter(patient, infirmierOrigine, infirmierDestination).then( (response) => {
            if(response.status === 200) {
                //console.log( "affecter patient", patient, infirmierOrigine, infirmierDestination);
                if(infirmierOrigine) {
                    infirmierOrigine.patients.splice( infirmierOrigine.patients.indexOf(patient), 1);
                } else {
                    // Il était dans le liste des patients non affectés
                    this.nf.patientsNonAffectes.splice( this.nf.patientsNonAffectes.indexOf(patient), 1);
                }
                if(infirmierDestination) {
                    infirmierDestination.patients.push( patient );
                } else {
                    // Il va dans la liste des patients non affectés
                    this.nf.patientsNonAffectes.push(patient);
                }
            }
        });
    }
    ngOnInit() {
        console.log("Appelez le service pour formatter et obtenir les données du cabinet\n", this);
        this.cms.getData( "/data/cabinetInfirmier.xml" ).then( (cabinet: NF.CabinetInterface) => {
            console.log( "cabinetJS:", cabinet );
            this.nf = cabinet;
            this.initDone = true;
        }, (err) => {console.error("Erreur lors du chargement du cabinet", "/data/cabinetInfirmier.xml", "\n", err);});
    }
    isPatient( obj ) {
        return obj.type === "patient" && obj.patient && obj.infirmierSource;
    }
};
