/**
 * @file
 * Specific availability behavior for detailed object page.
 */

Drupal.behaviors.tingAvailabilityTingObjectView = function () {
  "use strict";

  // Use get_details to load detailed data for each item on the page.
  Drupal.tingAvailability.get_details(function (data, textStatus) {
    // Update the standard status messages.
    Drupal.tingAvailability.updateStatus(data);

    // Inject data into the list of library that has the item available.
    if ($("#ting-object .ting-availability").length > 0) {
      $("#ting-object .ting-availability ul.library-list").empty();
      // Iterate over each library item's data.
      $.each(data, function (itemIndex, itemData) {
        var container;
        container = $('#ting-item-' + itemData.local_id + ' .ting-availability ul.library-list');

        // Find holdings, unique by library name.
        $.each(itemData.holdings, function (holdingIndex, holdingData) {
            container.append('<li>' + holdingData  + '</li>');
        });
        var headline = $('#ting-item-' + itemData.local_id + ' .ting-availability h3');
        if (itemData.total_count !== undefined &&
            itemData.reservable_count !== undefined &&
            itemData.reservable_count > 0 &&
            itemData.reserved_count !== undefined) {
          headline.after('<p>' + Drupal.t("There is @total_count copies available. @reservable_count can be reserved. There's @reserved_count reserved.", {
                '@total_count': itemData.total_count,
                '@reservable_count': itemData.reservable_count,
                '@reserved_count': itemData.reserved_count
                  }) + '</p>');
        }
        if (itemData.holdings.length === 0) {
          headline.text(Drupal.t("The material is currently not available at any library"));
        }
      });
    }
    else {
      // Remove container as no item is available
      $('.ting-availability ul.library-list').parent().remove();
    }
  });
  // Remove container as no ting id's where found
  if (Drupal.tingAvailability.id_list.length === 0) {
    $('.ting-availability ul.library-list').parent().remove();
  }
};

