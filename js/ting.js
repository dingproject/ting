
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
});

Drupal.ting.selectTab = function (event, ui) {
  // Set fragment, so tabs is bookmarkable.
  window.location.href = $(ui.tab).attr('href');
}
