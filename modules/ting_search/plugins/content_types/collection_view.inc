<?php
// $Id$

/**
 * @file collection.inc
 * Provides a Panels content type for display of Ting collections.
 */

/**
 * Callback function to supply a list of content types.
 *
 * This is only required to make CTools know that we're providing 
 * multiple types here (since we have no single => TRUE in the array).
 */
function ting_search_collection_view_ctools_content_types() {
  return array(
    'title' => t('Ting collection view'),
    'single' => TRUE,
    'description' => t('Provides the full display of a Ting collection.'),
    'required context' => new ctools_context_optional(t('Ting collection'), 'ting_collection'),
    'category' => t('Ting'),
  );
}

/**
 * Return the pane contents.
 */
function ting_search_collection_view_content_type_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();
  $collection = isset($context->data) ? drupal_clone($context->data) : NULL;

  if ($collection instanceOf TingClientObjectCollection) {
    // When viewing a Ting collection, redirect a single-item collection to
    // the object view for the item in question.
    if (count($collection->objects) == 1 && arg(0) == 'ting' && arg(1) == 'collection') {
      return drupal_goto('ting/object/' . $collection->objects[0]->id);
    }

    // If alma.module is present, use its JavaScript behaviors.
    if (function_exists('alma_add_js')) {
      alma_add_js('ting_collection_view');
    }
    $block->content = theme('ting_collection', $collection);
  }

  return $block;
}

/**
 * Return the pane titles.
 */
function ting_search_collection_view_content_type_admin_title($subtype, $conf, $context) {
  return t('“@s”: Ting collection view', array('@s' => $context->identifier));
}

/**
 * Returns an edit form for the content type.
 *
 * We're not actually doing anything here, but we need to have this hook 
 * for this content type to work.
 */
function ting_search_collection_view_content_type_edit_form(&$form, &$form_state) {
  return $form;
}

