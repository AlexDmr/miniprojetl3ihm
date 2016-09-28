//import { bootstrap }	                        from "@angular/platform-browser-dynamic";
//import { enableProdMode 	}               	from "@angular/core";
//import { disableDeprecatedForms, provideForms } from "@angular/forms";
import { NgModule }                 from "@angular/core";
import { BrowserModule }            from "@angular/platform-browser";
import { platformBrowserDynamic }   from "@angular/platform-browser-dynamic";
import { HttpModule }               from "@angular/http";

import {ComposantSecretaire}        from "./Components/ComposantSecretaire";

@NgModule({
    imports     : [BrowserModule, HttpModule],
    declarations: [ComposantSecretaire],
    bootstrap   : [ComposantSecretaire]
})
export class AppModule {}


platformBrowserDynamic().bootstrapModule(AppModule);

//enableProdMode();
