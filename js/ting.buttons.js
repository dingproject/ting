/**
* @file
 * Javascript helpers for Ting buttons.
 */
Drupal.tingButtons = {};

Drupal.tingButtons.dialogButton = function (selector, options) {
  "use strict";

  var self = this;

  self.defaults = {
    'buttons': function() {},
    'allowPropagation': false,
    'allowDefault': false
  };
  self.options = $.extend(self.defaults, options);
  self.selector = selector;

  // Set up routines.
  self.init = function () {
    $(self.selector).click(self.buttonClick);
  };

  // Button click handler.
  self.buttonClick = function (event) {
    self.clickEvent = event;

    // Do nothing if the user clicks on a disabled button.
    if (!$(this).hasClass('disabled')) {
      self.sendRequest(this.href);
    }

    // By default we prevent the browser from following the link and
    // and stop propagation as this used to be default behavior.
    // Now this can be changed through configuration.
    if (!self.options.allowPropagation) { event.stopPropagation(); }
    if (!self.options.allowDefault) { event.preventDefault(); }
  };

  // Generate the default buttons.
  self.defaultButtons = function () {
    var buttons = {};
    buttons[Drupal.t('Close')] = function () {
      $(this).dialog('close');
    };
  };

  // Generate the jQuery UI dialog response.
  self.generateDialog = function (title, message, buttons) {
    $('<div>' + message + '</div>')
      .dialog({
        'title': title,
        'buttons': buttons,
        'height': 'auto',
        'close': function (event, ui) {
          $(this).dialog('destroy').remove();
        }
      });
  };

  // Send the request to the server.
  self.sendRequest = function (url) {
    $.ajax({
      url: url,
      dataType: 'json',
      type: 'POST',
      cache: false,
      error: function (jqXHR) {
        var title = jqXHR.status + ' ' + jqXHR.statusText,
            message = Drupal.t('An error occurred. Please try again, or contact support if the problem persists.');

        self.generateDialog(title, message, self.defaultButtons());
      },
      success: function (data) {
        var buttons = self.defaultButtons(), message, title;

        // Message is overwritten by the data attribute.
        title = (data) ? data.title :  Drupal.t('An error occurred.');
        message = (data) ? data.message :  Drupal.t('An error occurred.');

        // If an e-book is available as alternative material, we want to
        // show that as an additional option.
        var eBookElem = $('.material-type-list .material-type-ebog');
        if (eBookElem.length > 0) {
          message = message + '<p>' +
              Drupal.t('The material you have reserved is also available as e-book. You can find more information <a href="@url">here</a>.', {
                '@url': eBookElem.find('a').attr('href')
              }) + '</p>';
        }

        self.options.buttons(buttons, self.clickEvent, data);
        self.generateDialog(title, message, buttons);
      }
    });
  };

  self.init();
};

