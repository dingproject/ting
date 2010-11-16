// $Id$

/**
 * Javascript helpers for Ting buttons.
 */
Drupal.tingButtons = {}

Drupal.tingButtons.dialogButton = function (selector, options) {
  defaults = { 'buttons': function() {} }
  var options = $.extend(defaults, options);

  $(selector).click(function (event) {
      // Make sure we grab the click.
      event.preventDefault();

      var $button = $(this);

      if (!$button.hasClass('disabled')) {
        // Make the request back to Drupal.
        $.post(this.href, {}, function (data) {
          var buttons, $count, message;
          // Message is overwritten by the data attribute.
          message = '';
          buttons = {};
          buttons[Drupal.t('Close')] = function () {
            $(this).dialog('close');
          };

          message = (data) ? data.message :  Drupal.t('An error occurred.');
          options.buttons(buttons, event, data);

          $('<div>' + message + '</div>')
            .dialog({
              'title': data.title,
              'buttons': buttons,
              'close': function (event, ui) {
                $(this).dialog('destroy').remove();
              }
            });
        }, 'json');
      }

      return false;
  });
};

