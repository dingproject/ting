
// Attache onClick to remove buttons in the admin UI.
Drupal.behaviors.tingProxyAdmin = function(context) {
  $(context).find('.remove').click(function () {
    var obj = $(this);
    // Mark as deleted and fix required
    obj.parent().find('.hidden-deleted').val(1);

    // Slide it up
    obj.parent().slideToggle('slow', function() {
      obj.parent().find('.url-text-field').val('deleted');
    });

    // Cancel normal submit
    return false;
  });
};
