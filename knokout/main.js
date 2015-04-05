"use strict";

$( document ).ready( function() {

    console.log( 'Hello, console!' );
    
    var myViewModel = {
        personName: 'Bob', // la UI non viene aggiornata sui cambiamenti
        personAge: ko.observable( 17 ) // la UI viene aggiornata sui cambiamenti
    };

    ko.applyBindings(myViewModel);

    // modifica un osservabile dopo 5 secondi
    setTimeout(function() {
        myViewModel.personAge( 18 );
    }, 5000);
    
});
