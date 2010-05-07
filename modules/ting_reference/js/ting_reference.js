Drupal.behaviors.addTingReferenceAutocomplete = function(context)
{
	type = jQuery('.ting-reference-type-radio:checked').val();
	path = Drupal.settings.tingReference.autocomplete[type];
	jQuery('input.ting-reference-autocomplete').each(function(i, e)
	{
		var autocompleter = jQuery(e);
		
		autocompleter.autocomplete(path, {});
		autocompleter.result(function(event, data, formatted)
		{
			jQuery(event.target).parent().siblings('.ting-object-id').val(data[1]).change();
		});

		autocompleter.parents('.form-ting-reference').find('input.ting-reference-type-radio').focus(function()
		{
			type = jQuery(this).val();
			path = Drupal.settings.tingReference.autocomplete[type];
			autocompleter.setOptions({ url: path });
			autocompleter.flushCache();
		});

	});

}

Drupal.behaviors.initPreview = function(context) {
  jQuery('input.ting-object-id').change(function() {
    var input = jQuery(this);
    var refType = input.parents('.form-ting-reference').find('input.ting-reference-type-radio:checked').val();
    jQuery.getJSON(
      Drupal.settings.tingReference.previewUrl + '/' + refType + '/' + Drupal.encodeURIComponent(input.val()), 
      null,
      function(data) {
        input.parents('.form-ting-reference').find('.ting-reference-preview').html(data);
      }
    );
  });

	jQuery('.ting-reference-reset').click(function(event) {
    jQuery(event.target).parents('.form-ting-reference').find('.ting-reference-preview').html('');
    jQuery(event.target).parents('.form-ting-reference').find('.ting-object-id').val('');
    // Do not submit the form.
    event.preventDefault();
  });
}
