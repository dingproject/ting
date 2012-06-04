
/**
 * Loads reference objects as HTML.
 */
Drupal.behaviors.tingReferenceAjax = function(context) {
  // Insert spinner(s) to show users that data is being loaded.
  $('.field-ting-refs span').after('<div class="ting-reference-ajax-spinner"><h4>' + Drupal.t('Loading…') + '</h4><div></div></div>');
  
  // This is a hack to get around the old 1.2.6 jquery limitations in the ajax 
  // post method.
  var rawdata = Drupal.settings.tingReferenceAjax;
  var data = {};
  for (var i = 0; i < rawdata.length; i++) {
    data['rows['+i+']'] = rawdata[i];
  }

  // Inserts the HTML return by the Ajax call below.
  function success(data, textStatus, jqXHR) {
    // Fill in data based on field name and node id.
    for (var nid in data) {
      for (var field_name in data[nid]) {
        // Find the element to place the result into.
        var field = $('.ting-reference-ajax-node-' + nid + '-' + field_name);

        // Hide the list.
        field.hide();

        // Remove spinner.
        $('.ting-reference-ajax-spinner', field).remove();

        // Insert the generated HTML.
        field.html(data[nid][field_name]);
        field.fadeIn(500);
      }
    }
  };

  // Call the backend to get referenced ting objects as rendered HTML. 
  $.ajax({
    type: 'POST',
    url: '/ting_reference/view/js',
    data: data,
    success: success,
    dataType: 'json'
  });
}