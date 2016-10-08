import {Injectable}     from "@angular/core";
import {Http, Response} from "@angular/http";
import "rxjs/add/operator/toPromise";

export enum sexeEnum {M, F}
export interface PatientInterface {
    prenom                  : string;
    nom                     : string;
    sexe                    : sexeEnum;
    numeroSecuriteSociale   : string;
    adresse                 : {
        ville       : string;
        codePostal  : number;
        rue         : string;
        numero      : string;
        etage       : string;
    };
}
export interface InfirmierInterface {
    id          : string;
    prenom      : string;
    nom         : string;
    photo       : string;
    patients    : PatientInterface[];
}
export interface CabinetInterface {
    infirmiers          : InfirmierInterface[];
    patientsNonAffectes : PatientInterface  [];
}

@Injectable()
export class ServiceCabinetMedical {
    cabinetJS: CabinetInterface;
    private _http : Http;
    constructor(http: Http) {
        this._http = http;
        this.cabinetJS = {
            infirmiers: [],
            patientsNonAffectes: []
        };
    }
    affecter( patient: PatientInterface,
              infirmierOrigine: InfirmierInterface,
              infirmierDestination: InfirmierInterface) : Promise<Response> {
        let infirmierId = infirmierDestination?infirmierDestination.id:"none";
        return this._http.post(
            "/affectation",
            {infirmier: infirmierId, patient: patient.numeroSecuriteSociale}
        ).toPromise();
    }
    getData( url: string ) : Promise<CabinetInterface> {
        return this._http.get(url).toPromise().then( (res: Response) => {
            // console.log( res.toString(), "\n", res.text() );
            let parser = new DOMParser();
            let doc = parser.parseFromString( res.text(), "text/xml" );
            if(doc) {
                // Créer les infirmiers
                let node : Element;
                for(let infirmierXML of doc.querySelectorAll( "infirmiers > infirmier" )) {
                    let infirmierJS : InfirmierInterface = {
                        id      : infirmierXML.getAttribute("id"),
                        prenom  : (node=infirmierXML.querySelector("prénom"))?node.textContent:"",
                        nom     : (node=infirmierXML.querySelector("nom")   )?node.textContent:"",
                        photo   : (node=infirmierXML.querySelector("photo") )?node.textContent:"",
                        patients: []
                    };
                    this.cabinetJS.infirmiers.push( infirmierJS );
                }

                // Créer les patients et les affecter aux infirmiers le cas échéant
                for(let patientXML of doc.querySelectorAll( "patients > patient" )) {
                    // Créer un patient
                    let patientJS : PatientInterface = {
                        prenom                  : (node=patientXML.querySelector("prénom"))?node.textContent:"",
                        nom                     : (node=patientXML.querySelector("nom")   )?node.textContent:"",
                        sexe                    : patientXML.querySelector("sexe").textContent==="M"?sexeEnum.M:sexeEnum.F,
                        numeroSecuriteSociale   : patientXML.querySelector("numéro").textContent,
                        adresse                 : {
                            ville       : (node=patientXML.querySelector("adresse > ville")     )?node.textContent:"",
                            codePostal  : (node=patientXML.querySelector("adresse > codePostal"))?parseInt(node.textContent):0,
                            rue         : (node=patientXML.querySelector("adresse > rue")       )?node.textContent:"",
                            numero      : (node=patientXML.querySelector("adresse > numéro")    )?node.textContent:"",
                            etage       : (node=patientXML.querySelector("adresse > étage")     )?node.textContent:""
                        }
                    };

                    // Trouver si il est affecté à un infirmier
                    let infirmierId : string = (node=patientXML.querySelector( "visite[intervenant]" ))?node.getAttribute("intervenant"):"";
                    if(infirmierId !== "") {
                        // Si oui alors l'affecter à cet infirmier
                        let infirmierJS = this.cabinetJS.infirmiers.find(
                            inf => inf.id === infirmierId
                        );
                        infirmierJS.patients.push(patientJS);
                    } else {
                        // Si non alors l'affecter aux patients restants
                        this.cabinetJS.patientsNonAffectes.push( patientJS);
                    }
                }
                return this.cabinetJS;
            }
            return null;
        });
    }
}
