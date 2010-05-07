<?php
// $Id$

/**
 * @file collection_id.inc
 *
 * Plugin to provide an argument handler for a Ting object id
 */

/**
 * Implementation of specially named hook_ctools_arguments().
 */
function ting_collection_id_ctools_arguments() {
  $args['collection_id'] = array(
    'title' => t("Ting collection"),
    'keyword' => 'collection_id',
    'description' => t('Creates a Ting collection context from a collection ID argument.'),
    'context' => 'ting_collection_id_context',
    'placeholder form' => array(
      '#type' => 'textfield',
      '#description' => t('Enter the collection ID for this argument'),
    ),
  );
  return $args;
}

/**
 * Discover if this argument gives us the node we crave.
 */
function ting_collection_id_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('ting_collection');
  }

  // We can accept either a collection object or a pure id.
  if (is_object($arg)) {
    return ctools_context_create('ting_collection', $arg);
  }

  if (empty($arg)) {
    // TODO: Can we do a better job of validating collection IDs?
    return FALSE;
  }

  module_load_include('client.inc', 'ting');
  $collection = ting_get_collection_by_id($arg, FALSE);
  if (!($collection instanceOf TingClientObjectCollection) || empty($collection->objects)) {
    return FALSE;
  }

  return ctools_context_create('ting_collection', $collection);
}

