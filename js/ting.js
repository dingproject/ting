
/**
 * @file
 * JavaScript file holding most of the ting view related functions.
 */

/**
 * Set up the ting page.
 *
 * This is _not_ a Drupal.behavior, since those take a lot longer to load.
 */

Drupal.ting = {};

$(function () {
  // Configure our tabs
  $('.tab-navigation')
    .tabs( {
        select: Drupal.ting.selectTab
   });
   
   // Allow users to show/hide object details
   Drupal.ting.toggleDetails();
});

Drupal.ting.selectTab = function (event, ui) {
  // Set fragment, so tabs are bookmarkable.
  window.location.href = $(ui.tab).attr('href');
}

Drupal.ting.toggleDetails = function () {
  $('.object-information .additional-details > .item-list')
    // Hide additional details by default
    .hide()
    // Add controls to show/hide additional details...
    .before('<a href="#">' + Drupal.t('Show details') + '</a>')
    .before('<a style="display:none;" href="#">' + Drupal.t('Hide details') + '</a>')
    // ... and make them work
    .siblings('a').click(function(e) {
      $(this)
        .toggle()
        .siblings().toggle()
      e.preventDefault();
    });
}
