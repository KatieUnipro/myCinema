jQuery(document).ready(function($){
   console.log('I am working');
   var wrapper = $(".header-wrapper");
   wrapper.sticky({topSpacing:0});

   wrapper.on('sticky-start', function() { wrapper.addClass('opacity')});
   wrapper.on('sticky-end', function() {  wrapper.removeClass('opacity')});
});