/**
 * JavaScript behaviors for Ting reference form element.
 */

/**
 * Set up the autocomplete for Ting search.
 */
Drupal.behaviors.addTingReferenceAutocomplete = function(context) {
  var path, type;
	type = jQuery('.ting-reference-type-radio:checked').val();
	path = Drupal.settings.tingReference.autocomplete[type];
	jQuery('input.ting-reference-autocomplete').each(function(index, element) {
		var autocompleter = jQuery(element);
		
		autocompleter.autocomplete(path, {});
		autocompleter.result(function(event, data, formatted) {
			jQuery(event.target).parent().siblings('.ting-object-id').val(data[1]).change();
		});

		autocompleter.parents('.form-ting-reference').find('input.ting-reference-type-radio').focus(function() {
			type = jQuery(this).val();
			path = Drupal.settings.tingReference.autocomplete[type];
			autocompleter.setOptions({ url: path });
			autocompleter.flushCache();
		});

	});
};

/**
 * Set up the preview for the currently selected object.
 */
Drupal.behaviors.tingReferencePreview = function(context) {
  var wrapper = jQuery('.form-item.form-ting-reference');

  // Whenever the object ID changes, refresh the prison.
  wrapper.find('.ting-object-id').change(function() {
    var input = jQuery(this), refType;
    refType = input.parents('.form-ting-reference').find('input.ting-reference-type-radio:checked').val();

    jQuery.getJSON(
      Drupal.settings.tingReference.previewUrl + '/' + refType + '/' + Drupal.encodeURIComponent(input.val()), 
      null,
      function(data) {
        input.parents('.form-ting-reference').find('.ting-reference-preview').html(data);
      }
    );
  });

  // Add a reset link.
  wrapper.find('.ting-reference-preview').after('<a class="ting-reference-reset" href="#reset">' + Drupal.t('Reset') + '</a>');

  // Enable the reset link.
	wrapper.find('.ting-reference-reset').click(function (event) {
    jQuery(event.target)
      .parents('.form-ting-reference')
        .find('.ting-reference-preview')
          .html('')
        .end()
        .find('.ting-object-id')
          .val('');

    // Do not follow the link.
    return false;
  });
};

