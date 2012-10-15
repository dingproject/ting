/**
 * @file alma.availability.js
 * JavaScript behaviours for fetching and displaying availability.
 */

// Container object for all our availability stuff.
Drupal.tingAvailability = {
  id_matcher: /ting-item-(\d+)/,
  id_list: []
};

/**
 * Helper function to find and store all ting item ids.
 */
Drupal.tingAvailability.find_ids = function () {
  "use strict";

  $("div.ting-item").each(function () {
    var match = Drupal.tingAvailability.id_matcher.exec(this.id);

    if (match) {
      Drupal.tingAvailability.id_list.push(match[1]);
    }
  });
};

/**
 * Get details for all ting items found.
 */
Drupal.tingAvailability.get_details = function (callback) {
  "use strict";
  var ajax_path;

  if (Drupal.settings.trampolinePath) {
    ajax_path = Drupal.settings.trampolinePath;
  }
  else {
    ajax_path = Drupal.settings.basePath;
  }

  // If the id_list is empty, try and find ids again.
  if (Drupal.tingAvailability.id_list.length === 0) {
    Drupal.tingAvailability.find_ids();
  }

  if (Drupal.tingAvailability.id_list.length > 0) {
    $.getJSON(ajax_path + 'ting/availability/item/' + Drupal.tingAvailability.id_list.join(',') + '/details', {}, callback);
  }
};

/**
 * Get availability for all ting items found.
 *
 * This call is more light-weight than get_details, and thus more
 * suitable if you have multiple ting items on a page.
 */
Drupal.tingAvailability.get_availability = function (callback) {
  "use strict";
  var ajax_path;

  // If the id_list is empty, try and find ids again.
  if (Drupal.tingAvailability.id_list.length === 0) {
    Drupal.tingAvailability.find_ids();
  }

  if (Drupal.tingAvailability.id_list.length > 0) {
    if (Drupal.settings.trampolinePath) {
      ajax_path = Drupal.settings.trampolinePath;
    }
    else {
      ajax_path = Drupal.settings.basePath;
    }
    $.getJSON(ajax_path + 'ting/availability/item/' + Drupal.tingAvailability.id_list.join(',') + '/details', {}, callback);
  }
};

/**
 * Availability information for all pages.
 *
 * Try to find Ting items and stuff availability data into them.
 */
Drupal.tingAvailability.updateStatus = function (data, textStatus) {
  "use strict";

  $.each(data, function(itemId, itemData) {
    var $item = $('#ting-item-' + itemId);
    if (!itemData.show_reservation_button) {
      $item.find('.ting-availability-status')
        .addClass('unreservable')
        .removeClass('waiting')
        .text(Drupal.t('not reservable'))
      .end()
      // FIXME: There are two problems here:
      // - Reservation adds a class to the link. Cart to the li.
      //   The class for the link should be moved to the li in
      //   ding_reservation_ting_object_buttons().
      // - The carting button is toggled based on whether the item
      //   is reservable or not. This it harmful to sites which
      //   offer other options besides reservation from the cart.
      .find('.ting-status ul.buttons > li > a.reserve-now,' +
            '.ting-status ul.buttons > li.add-to-cart > a')
        .addClass('disabled');
    }
    else if (itemData.available_from) {
      $item.find('.ting-availability-status')
        .addClass('unavailable')
        .removeClass('waiting')
        .text(Drupal.t('available from @date', {'@date': itemData.available_from}));
    }
    else if (itemData.deferred_period) {
      $item.find('.ting-availability-status')
        .addClass('unavailable')
        .removeClass('waiting')
        .text(Drupal.t('waiting period'));
    }
    else if (itemData.available) {
      $item.find('.ting-availability-status')
        .addClass('available')
        .removeClass('waiting')
        .text(Drupal.t('available'));
    }
    else if (!itemData.reservable) {
      $item.find('.ting-availability-status')
        .addClass('unavailable')
        .removeClass('waiting')
        .text(Drupal.t('unavailable'));
    }
    else if (itemData.reserved_count < 1) {
      $item.find('.ting-availability-status')
        .addClass('unavailable')
        .removeClass('waiting')
        .text(Drupal.t('on loan'));
    } else {
      $item.find('.ting-availability-status')
        .addClass('unavailable')
        .addClass('reserved')
        .removeClass('waiting')
        .text(Drupal.t('reserved'));
    }
  });
};
