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
function ting_collection_view_ctools_content_types() {
  return array(
    'title' => t('Ting collection view'),
    'single' => TRUE,
    'description' => t('Provides the full display of a Ting collection.'),
    'required context' => array(new ctools_context_optional(t('Ting collection'), 'ting_collection'),
                                new ctools_context_optional(t('Query arguments'), 'string'),
                                ),
    'category' => t('Ting'),
  );
}

/**
 * Return the pane contents.
 */
function ting_collection_view_content_type_render($subtype, $conf, $panel_args, $context) {
  $block = new stdClass();

  $collection = isset($context[0]->data) ? drupal_clone($context[0]->data) : NULL;

  $query_args = isset($context[1]->data) ? _ting_search_parse_query_args($context[1]->data) : NULL;

  if ($collection instanceOf TingClientObjectCollection) {
    // When viewing a Ting collection, redirect a single-item collection to
    // the object view for the item in question.
    if (count($collection->objects) == 1 && arg(0) == 'ting' && arg(1) == 'collection') {
      return drupal_goto('ting/object/' . $collection->objects[0]->id);
    }

    $block->content = theme('ting_thing', 'collection', $collection);
  }

  return $block;
}

/**
 * Return the pane titles.
 */
function ting_collection_view_content_type_admin_title($subtype, $conf, $context) {
  return t('“@s1, @s2”: Ting collection view', array('@s1' => $context[0]->identifier, '@s2' => $context[1]->identifier));
}

/**
 * Returns an edit form for the content type.
 *
 * We're not actually doing anything here, but we need to have this hook
 * for this content type to work.
 */
function ting_collection_view_content_type_edit_form(&$form, &$form_state) {
  return $form;
}

