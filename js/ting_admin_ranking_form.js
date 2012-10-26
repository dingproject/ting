/**
 * @file
 * JavaScript behaviors for ranking field editing form.
 */

(function($) {
  "use strict";

  // Add remove-button to each ranking field.
  Drupal.behaviors.tingAdminRankingForm = function(context) {
    $(context).find('.ranking-field-instance').each(function () {
      var field = $(this);

      $('<input class="form-submit" name="remove" type="button" value="' + Drupal.t('Remove') + '" />')
        .click(function () {
          // Setting an empty field name will cause the field to be
          // deleted on the server.
          field.find('select.field-name').val('');

          // Hide the deleted field.
          field.parent().slideUp('fast');
          return false;
        })
        .appendTo(field);
    });
  };

}(jQuery));
