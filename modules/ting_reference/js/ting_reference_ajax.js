
/**
 * Loads reference objects as HTML.
 */
Drupal.behaviors.tingReferenceAjax = function(context) {
  // Insert spinner to show users that data is being loaded.
  $('.field-ting-refs span').after('<div id="ting-search-spinner"><h4>' + Drupal.t('Loading…') + '</h4><div class="spinner"></div></div>');
  
  // This is a hack to get around the old 1.2.6 jquery limitations in the ajax 
  // post method.
  var rawdata = Drupal.settings.tingReferenceAjax;
  var data = {};
  for (var i = 0; i < rawdata.length; i++) {
    data['rows['+i+']'] = rawdata[i];
  }

  // Inserts the HTML return by the Ajax call below.
  function success(data, textStatus, jqXHR) {
    // Hide the list.
    $('.field-ting-refs').hide();

    // Remove spinner.
    $('#ting-search-spinner').remove();    

    // Show the list with the new items.
    $('.field-ting-refs span').after(data);
    $('.field-ting-refs').fadeIn(500);    
  };

  // Call the backend to get referenced ting objects as rendered HTML. 
  $.ajax({
    type: 'POST',
    url: '/ting_reference/view/js',
    data: data,
    success: success,
    dataType: 'html'
  });
}