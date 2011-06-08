/**
 * @file
 * Javascript helpers for Ting buttons.
 */
Drupal.tingButtons = {};

Drupal.tingButtons.dialogButton = function (selector, options) {
  var self = this;

  self.defaults = { 'buttons': function() {},
                    'allowPropagation': false,
                    'allowDefault': false };
  self.options = $.extend(self.defaults, options);
  self.selector = selector;

  /**
   * Set up routines.
   */
  self.init = function () {
    $(self.selector).click(self.buttonClick);
  };

  /**
   * AJAX error handler.
   */
  self.ajaxErrorCallback = function (jqXHR, textStatus, errorThrown) {
    var title = jqXHR.status + ' ' + jqXHR.statusText,
        message = Drupal.t('An error occurred. Please try again, or contact support if the problem persists.');
    
    self.generateDialog(title, message, self.defaultButtons());
  };

  /**
   * AJAX success handler.
   */
  self.ajaxSuccessCallback = function (data, textStatus, jqXHR) {
    var buttons = self.defaultButtons(), message, title;

    // Message is overwritten by the data attribute.
    title = (data) ? data.title :  Drupal.t('An error occurred.');
    message = (data) ? data.message :  Drupal.t('An error occurred.');
    self.options.buttons(buttons, self.clickEvent, data);
    self.generateDialog(title, message, buttons);
  };

  /**
   * Button click handler.
   */
  self.buttonClick = function (event) {
    self.clickEvent = event;
    if (!$(this).hasClass('disabled')) {
      $.ajax({
        url: this.href,
        dataType: 'json',
        type: 'POST',
        cache: false,
        error: self.ajaxErrorCallback,
        success: self.ajaxSuccessCallback
      });
    } //Do nothing if the user clicks on a disabled button
    
    // By default we prevent the browser from following the link and
    // and stop propagation as this used to be default behavior.
    // Now this can be changed through configuration.
    if (!self.options.allowPropagation) {
      event.stopPropagation();
    }
    if(!self.options.allowDefault) {
      event.preventDefault();
    }
  };

  /**
   * Generate the default buttons.
   */
  self.defaultButtons = function () {
    var buttons = {};
    buttons[Drupal.t('Close')] = function () {
      $(this).dialog('close');
    };
  };

  /**
   * Generate the jQuery UI dialog response.
   */
  self.generateDialog = function (title, message, buttons) {
    $('<div>' + message + '</div>')
      .dialog({
        'title': title,
        'buttons': buttons,
        'close': function (event, ui) {
          $(this).dialog('destroy').remove();
        }
      });
  };

  self.init();
};

