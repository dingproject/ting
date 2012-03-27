/**
 * Set up the preview for the currently selected object.
 */
Drupal.behaviors.tingReferenceAjax = function(context) {
  var tingObjects = Drupal.settings.tingReferenceAjax;
  var placeHolders = $('.field-ting-refs [class^=object-id-]')
  console.log(placeHolders.length);
  
}