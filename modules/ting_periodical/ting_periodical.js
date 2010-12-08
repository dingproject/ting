/**
 * @file
 * Ting periodical functions.
 */

/**
 * Year foldout behaviour.
 */
Drupal.behaviors.tingPeriodical = function (context) {
  var years = $('#periodical-issue-list .year');
  if (years.length > 1) {
    // Hide all an click event
    years.each( function () {
      $('ul', this).hide();
      $(this).toggle(function () {
        $('ul', this).show();
        $(this).addClass("expanded");
      }, function() {
        $('ul', this).hide();
        $(this).removeClass("expanded")
      })
      .css('cursor', 'pointer');
    });
  }

};
