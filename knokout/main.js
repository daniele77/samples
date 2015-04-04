$( document ).ready(function() {
    console.log( 'Hello, console!' )
    
    var myViewModel = {
        personName: 'Bob',
        personAge: 123
    };

    ko.applyBindings(myViewModel);

});
