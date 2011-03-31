<?php

/**
 * @file object_id.inc
 *
 * Plugin to provide an argument handler for a Ting object id
 */

/**
 * Implementation of specially named hook_ctools_arguments().
 */
function ting_object_id_ctools_arguments() {
  $args['object_id'] = array(
    'title' => t("Ting object"),
    'keyword' => 'object_id',
    'description' => t('Creates a Ting object context from a object ID argument.'),
    'context' => 'ting_object_id_context',
    'placeholder form' => array(
      '#type' => 'textfield',
      '#description' => t('Enter the object ID for this argument'),
    ),
  );
  return $args;
}

/**
 * Discover if this argument gives us the node we crave.
 */
function ting_object_id_context($arg = NULL, $conf = NULL, $empty = FALSE) {
  // If unset it wants a generic, unfilled context.
  if ($empty) {
    return ctools_context_create_empty('ting_object');
  }

  // We can accept either a node object or a pure nid.
  if (is_object($arg)) {
    return ctools_context_create('ting_object', $arg);
  }

  if (empty($arg)) {
    // TODO: Can we do a better job of validating object IDs?
    return FALSE;
  }

  module_load_include('client.inc', 'ting');
  $object = ting_get_object_by_id($arg, TRUE);
  if (!($object instanceOf TingClientObject)) {
    return FALSE;
  }

  return ctools_context_create('ting_object', $object);
}

