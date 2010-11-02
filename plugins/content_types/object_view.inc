<?php
// $Id$

/**
 * @file object.inc
 * Provides a Panels content type for display of Ting objects.
 */

/**
 * Callback function to supply a list of content types.
 *
 * This is only required to make CTools know that we're providing 
 * multiple types here (since we have no single => TRUE in the array).
 */
function ting_object_view_ctools_content_types() {
  return array(
    'title' => t('Ting object view'),
    'single' => TRUE,
    'description' => t('Provides the full display of a Ting object.'),
    'required context' => new ctools_context_optional(t('Ting object'), 'ting_object'),
    'category' => t('Ting'),
  );
}

/**
 * Return the pane contents.
 */
function ting_object_view_content_type_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();
  $object = isset($context->data) ? drupal_clone($context->data) : NULL;

  if ($object instanceOf TingClientObject) {
    // Allow modules to add javascript handlers to the view
    module_invoke_all('ting_add_js', 'ting_object_view');

    $block->content = theme('ting_object', $object);
  }

  return $block;
}

/**
 * Return the pane titles.
 */
function ting_object_view_content_type_admin_title($subtype, $conf, $context) {
  return t('“@s”: Ting object view', array('@s' => $context->identifier));
}

/**
 * Returns an edit form for the content type.
 *
 * We're not actually doing anything here, but we need to have this hook 
 * for this content type to work.
 */
function ting_object_view_content_type_edit_form(&$form, &$form_state) {
  return $form;
}

