/**
 * @file
 * Specific availability behavior for detailed object page.
 */

Drupal.behaviors.tingAvailabilityTingObjectView = function () {
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
          // If the total count for the library is bigger than the
          // number that library has checked out, it is interpreted as
          // if the item is available.
          // This because available_count is really the number available
          // for reservation, and doesn't count things that can be
          // loaned, but not reserved (14 day loans, etc.).
          // if (holdingData.total_count > holdingData.checked_out_count) {
            container.append('<li>' + holdingData  + '</li>');
            //}
        });
        // If holding is empty remove container
        if (itemData.holdings.length === 0) {
          container.parent().remove();
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

