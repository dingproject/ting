<?php
/**
 * @file
 * Page callbacks for the Ting module.
 */

/**
 * Redirect to object view based on local ID (used by bibliotek.dk).
 */
function ting_search_pages_object_redirect($local_id) {
  module_load_include('client.inc', 'ting');

  $object = ting_get_object_by_local_id($local_id);
  if (!$object) {
    return drupal_not_found();
  }
  drupal_goto($object->url);
}

