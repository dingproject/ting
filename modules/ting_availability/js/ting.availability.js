// $Id$
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
  // If the id_list is empty, try and find ids again.
  if (Drupal.tingAvailability.id_list.length === 0) {
    Drupal.tingAvailability.find_ids();
  }

  if (Drupal.tingAvailability.id_list.length > 0) {
    $.getJSON(Drupal.settings.basePath + 'ting/availability/item/' + Drupal.tingAvailability.id_list.join(',') + '/details', {}, callback);
  }
};

/**
 * Get availability for all ting items found.
 *
 * This call is more light-weight than get_details, and thus more
 * suitable if you have multiple ting items on a page.
 */
Drupal.tingAvailability.get_availability = function (callback) {
  // If the id_list is empty, try and find ids again.
  if (Drupal.tingAvailability.id_list.length === 0) {
    Drupal.tingAvailability.find_ids();
  }

  if (Drupal.tingAvailability.id_list.length > 0) {
    $.getJSON(Drupal.settings.basePath + 'ting/availability/item/' + Drupal.tingAvailability.id_list.join(','), {}, callback);
  }
};

/**
 * Availability information for all pages.
 *
 * Try to find Ting items and stuff availability data into them.
 */
Drupal.tingAvailability.updateStatus = function (data, textStatus) {
  $.each(data, function(itemId, itemData) {
    var $item = $('#ting-item-' + itemId);
    if (!itemData.show_reservation_button) {
      $item.find('.ting-status')
        .addClass('unreservable')
        .removeClass('waiting')
        .text(Drupal.t('not reservable'))
      .end()
      .find('ul.ding-cart-buttons > li > a')
        .addClass('disabled');
    }
    else if (itemData.available_count > 0) {
      $item.find('.ting-status')
        .addClass('available')
        .removeClass('waiting')
        .text(Drupal.t('available'));
    }
    else if (itemData.reservation === 0) {
      $item.find('.ting-status')
        .addClass('unavailable')
        .removeClass('waiting')
        .text(Drupal.t('unavailable'));
    }
    else {
      $item.find('.ting-status')
        .addClass('unavailable')
        .addClass('reserved')
        .removeClass('waiting')
        .text(Drupal.t('reserved'));
    }
  });
};

/**
 * Format a holding as readable text.
 */
Drupal.tingAvailability.formatHolding = function (item, holding) {
  if (!Drupal.settings.ting_availability.organisation) {
    return '';
  }
	
  var locations = [Drupal.settings.ting_availability.organisation.branches[holding.branch_id]], output;

  // Take each location type ID and look it up in our department data.
  // If a match is found, add it to the locations array.
  $.each(['department', 'location', 'sublocation', 'collection'], function (index, location_type) {
    var location_id = holding[location_type + '_id'], location_name;

    if (location_id) {
      location_name = Drupal.settings.ting_availability.organisation[location_type + 's'][location_id];
    }

    if (location_name) {
      locations.push(location_name);
    }
  });

  output = locations.join(' → ');

  // Shelf mark includes a '>', so we add it after joining with arrows.
  if (holding.shelf_mark) {
    output += (holding.shelf_mark);
  }

  return output;
};

